# M01: Lifetimes: stack, static, heap

## Advance organizer

Every object in a C program has a **storage duration**: the stretch of execution during
which its bytes are reserved for it and its address means something. C gives you three you
will use constantly:

| Storage duration | How you get it | Starts | Ends |
|---|---|---|---|
| automatic ("stack") | a local variable without `static` | entry to its block | exit from its block |
| static | file-scope variable, or a local declared `static` | before `main` runs | after `main` returns |
| allocated ("heap") | `malloc`, `calloc`, `realloc`, and POSIX `strdup` (ISO C only since C23) | the call returns non-NULL | the matching `free` |

Everything in this course is a consequence of one rule: **a pointer is only usable while
the object it points to is alive.** The pointer variable itself has its own lifetime, and
the two are unrelated. A pointer can outlive its target (dangling), and a target can
outlive every pointer to it (leaked, if it is on the heap). This unit is about choosing
the right lifetime for the data a function produces. The heap is one of three options,
and often not the best one.

If you did `LearnCwithRPG-terminal` ch10, you have seen the stack/heap picture. This unit
adds static storage and gives you a decision procedure.

## Foundations

### Automatic storage is tied to a block, not to a function call alone

```c
int *f(void) {
    int x = 42;      /* x: automatic, lives from here ... */
    return &x;       /* ... until the closing brace of f   */
}
```

When `f` returns, its frame is popped. The 4 bytes that held `x` are still physically
there, but they no longer belong to any object. The caller holds an address into a region
the next function call will reuse for *its* locals. The standard calls this an indeterminate
pointer value. Using it is undefined behaviour, and you can't fix that by being careful
about timing.

The same holds for inner blocks: a variable declared inside `{ ... }` of a `for` loop dies
at that loop body's closing brace, every iteration.

### Static storage lives for the whole run, which makes it shared

```c
const char *label(int n) {
    static char buf[32];
    snprintf(buf, sizeof buf, "item-%d", n);
    return buf;
}
```

`buf` has exactly one instance for the whole program. Returning its address is legal: the
object never dies. The cost is that **every call writes into the same 32 bytes**.
`label(1)` and `label(2)` return the *same pointer*, so

```c
printf("%s %s\n", label(1), label(2));
```

prints the same string twice (which one depends on the unspecified order in which the
arguments are evaluated). Standard functions with this design include `strerror`
(historically), `asctime`, `ctime`, `inet_ntoa`, `getenv` (whose result a later `setenv` may overwrite),
and `strtok`'s hidden state. Static storage also makes a function non-reentrant and unsafe
to call from two threads.

### Allocated storage lives until someone frees it

```c
char *label_heap(int n) {
    char *buf = malloc(32);
    if (buf == NULL) return NULL;
    snprintf(buf, 32, "item-%d", n);
    return buf;           /* caller now owns 32 bytes and must free them */
}
```

Each call produces a distinct 32-byte block. The caller can keep as many as it likes. The
price is two new obligations: the call can fail (NULL), and somebody must call `free`
exactly once. M02 onward is about meeting those obligations.

### The fourth option: let the caller provide the storage

```c
char *label_into(char *dst, size_t cap, int n) {
    snprintf(dst, cap, "item-%d", n);
    return dst;
}
```

The callee allocates nothing. The caller picks the lifetime: a local array, a static
array, or a heap block. This is the design `snprintf`, `fgets`, and POSIX `strerror_r` use (glibc's GNU `strerror_r` may instead return a static string and leave your buffer unfilled). It is
usually the best default for small, bounded results, because it cannot leak and cannot fail
for lack of memory.

### Decision procedure

Ask in this order:

1. Is the size known and small at compile time, and does the result only need to live as
   long as the caller's frame? Use **caller-provided storage**.
2. Is there exactly one of it for the whole program, and is it never needed in two places
   at once? **static** is acceptable. Document that the next call overwrites it.
3. Is the size known only at run time, or must the result outlive the caller's frame, or
   must many independent copies coexist? Use the **heap**, and document who frees it.

## Worked example 1: the returned local

```c
#include <stdio.h>

static int *make_counter(void) {
    int count = 0;
    return &count;                 /* gcc: -Wreturn-local-addr */
}

static void noisy(void) {
    volatile int scratch[8] = {7, 7, 7, 7, 7, 7, 7, 7};
    (void)scratch;
}

int main(void) {
    int *c = make_counter();
    noisy();
    printf("%d\n", *c);
}
```

What happens in memory: `make_counter`'s frame holds `count`. On return that frame is
released. `c` in `main` holds an address inside the released region. `noisy` is then
called and its frame occupies the same region, so `scratch` may land exactly where `count`
was.

**Rejected reading:** "It prints 0 because nothing has overwritten `count` yet" or "it
prints 7 because `noisy` overwrote it." Neither is guaranteed, and that is the point.
Modern gcc goes further: when it can prove you return the address of a local, it warns and
**returns a null pointer instead**. Then `*c` is a segfault, not a stale value. The output
depends on compiler, optimisation level and flags. That is what undefined behaviour means
in practice. A program you can't reason about from the source is wrong even if it happens
to print the right thing today.

## Worked example 2: two static results alive at once

```c
const char *fmt_hp(int hp) {
    static char buf[16];
    snprintf(buf, sizeof buf, "HP %d", hp);
    return buf;
}

const char *a = fmt_hp(10);
const char *b = fmt_hp(99);
/* a == b, and both point at "HP 99" */
```

In memory there is one 16-byte object in the static data segment. `a` and `b` are two
pointer variables on `main`'s stack holding the same address. The second call rewrote the
bytes the first result referred to.

**Rejected reading:** "`a` still says `HP 10` because it was assigned first." `a` never held
a string. It held an address, and the content at that address changed. Nothing here is undefined
behaviour (the object is alive), so no sanitizer will ever flag it. It is a logic bug that only
design prevents. That is why the M01 exercises treat static buffers as a trade-off you write
down rather than a bug a tool finds.

## Where tools help and where they don't

- gcc warns for the simple returned-local case even without `-Wall` (`-Wreturn-local-addr`
  is on by default). Laundered versions are hit and miss: through a second local pointer it
  warns only at `-O2`, and through a struct field it doesn't warn at all (e03 measures this).
- ASan can catch a use of a dead stack frame only with
  `ASAN_OPTIONS=detect_stack_use_after_return=1`. M11 revisits this unit with that flag.
- No tool flags the static-buffer overwrite. It is legal C.

## man pointers

`man` is reference, not the lesson. Look these up after you have done the exercises:

- `man 3 strerror`: compare `strerror` and `strerror_r` and see which one returns a buffer it owns.
- `man 3 ctime`: NOTES section, on the static result buffer and the `_r` variants.
- `man gcc`, search for `-Wreturn-local-addr` and `-Wdangling-pointer`.

## Check yourself

1. A function declares `char buf[64]` and returns `buf`. Name the storage duration, the
   moment the object dies, and what the caller's pointer holds afterwards.
2. Why is returning a pointer to a `static` local legal when returning a pointer to an
   automatic local is not?
3. A function returns a pointer to a string literal (`return "ok";`). What is the storage
   duration of the literal, and what may the caller *not* do with it?
4. Give one situation where caller-provided storage is a worse choice than the heap.
5. `int *p = malloc(sizeof *p);` inside a function that returns without freeing or
   returning `p`. What is alive after the return, and what is not?

<details><summary>Answers</summary>

1. Automatic. It dies when the function returns. The caller's pointer is indeterminate: gcc
   even replaces the returned address with a null pointer, so using it crashes or worse.
2. A `static` local has static storage duration: it lives for the whole program, so the
   address stays valid. An automatic local dies at return.
3. Static storage duration. The caller must not modify it (UB) and must not `free` it.
4. When the size is only known inside the callee (e.g. reading a line of unknown length),
   or when the result must outlive the caller's frame.
5. The heap block is alive, because nothing freed it. `p` died at return, so the block is
   unreachable: a leak.

</details>

## Takeaways

- A pointer and the object it points to have independent lifetimes. Track both.
- Automatic objects die at the closing brace of their block, with no exceptions.
- Static results are always valid but always shared. Document the overwrite.
- Heap results are independent and long-lived but can fail and must be freed once.
- Caller-provided storage is the default for small bounded outputs.

## Exercises

| # | Folder | Tag | Size | Topic |
|---|---|---|---|---|
| e01 | `e01-trace-frames/` | `[TRACE]` | S | Draw stack frames and a static object at three marked points |
| e02 | `e02-return-local/` | `[BUG]` | S | A helper returns `&local`; fix the lifetime and explain the mechanism |
| e03 | `e03-predict-warning/` | `[PREDICT]` | S | Predict which functions `-Wreturn-local-addr` warns about, and which it misses |
| e04 | `e04-three-ways/` | `[WRITE]` | S | One formatting function, written three ways: static, caller buffer, heap |
| e05 | `e05-static-overwrite/` | `[EXPLAIN]` | S | Explain why a program using a static-buffer formatter prints the wrong thing |

`[EXPLAIN]` is a `[PREDICT]`-family exercise: a complete, buildable program plus a written
answer in `ANSWER.md`. There is nothing to implement.
