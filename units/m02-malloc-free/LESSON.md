# M02: `malloc` and `free` fundamentals

## Advance organizer

M01 ended with "use the heap when the size is only known at run time, or the result must
outlive the caller's frame." This unit covers the four rules every heap allocation in this
course follows:

1. Size it with `sizeof *p`, never with a repeated type name.
2. Check the result against NULL before the first use.
3. Free it exactly once, after the last use.
4. After `free(p)`, treat `p` as poison. Set it to NULL if it stays in scope.

You will break each rule in the exercises. M02b, next, teaches you to read the reports
the tools print when you do.

## Foundations

### What `malloc(n)` gives you

`malloc(n)` either returns NULL, or returns a pointer to a block of **at least** `n` bytes
that:

- is suitably aligned for any type with fundamental alignment (`alignof(max_align_t)`, 16 bytes on x86-64 glibc);
- has **indeterminate contents**, so reading a byte before writing it is a bug. Fresh blocks
  from a fresh process often happen to be zero, and recycled blocks often aren't. Neither is
  a promise;
- belongs to no other object, and stays yours until you pass it to `free` (or `realloc`).

glibc's allocator (others differ) keeps its own bookkeeping (size, free-list links) in a
header just before the address it gives you. You never see that header, but you can destroy it: writing past
the end of your block, or before its start, overwrites the neighbouring block's header.
That's why heap overflows tend to crash later, inside an unrelated `malloc` or `free`.

### `sizeof *p`

```c
struct monster *m = malloc(sizeof *m);
double *xs = malloc(n * sizeof *xs);
```

`sizeof` does not evaluate its operand (except for variable-length arrays), so `*m` is
never dereferenced. The expression just asks "how big is the thing `m` points to?" If `m`'s type
later changes, the allocation stays correct. With `malloc(sizeof(struct monster))`,
the type name can drift out of sync with the declaration. The classic slip, `sizeof m`
instead of `sizeof *m`, allocates 8 bytes (the size of a pointer). M12 makes you hunt it.

Don't cast the result. `void *` converts implicitly in C. The cast adds nothing and once hid
a missing `#include <stdlib.h>`.

### NULL checks

On Linux, small `malloc` calls rarely return NULL because of overcommit, but they can:
under `ulimit -v`, in containers with memory limits, for huge sizes, and always under this
course's fault injector (`make fault`, from M04). The check goes right after the call, and
the failure path must leave the program in a state you can describe:

```c
int *v = malloc(n * sizeof *v);
if (v == NULL) {
    fprintf(stderr, "out of memory\n");
    exit(EXIT_OOM);          /* 3, from lib/course.h */
}
```

In library-style functions, return NULL (or an error code) and let the caller decide.
Exiting is the caller's call, usually `main`'s.

### `free`

- `free(p)` returns the block to the allocator. The block's **lifetime ends**. Every
  pointer that held that address is now dangling. Not just `p`, but every copy of it.
- `free(NULL)` is defined to do nothing. Don't write `if (p) free(p);`.
- Passing anything else (a stack address, an interior pointer such as `p + 1`, a block
  already freed) is undefined. glibc often aborts with a message, and sometimes it doesn't.
- `free` cannot set your pointer to NULL. It receives a copy of the address. Do it yourself:
  `free(p); p = NULL;`. This turns a later use-after-free into a NULL dereference (a clean
  crash) and a later double free into `free(NULL)` (harmless). It does nothing for the
  *other* copies of the address. Ownership discipline (M05) handles those.

## Worked example 1: the stale copy

```c
int *a = malloc(4 * sizeof *a);
if (a == NULL) exit(EXIT_OOM);
for (int i = 0; i < 4; i++) a[i] = i * 10;

int *view = a;      /* second pointer to the same 16-byte block */
free(a);
a = NULL;

printf("%d\n", view[2]);
```

In memory: one 16-byte heap block, call it B. `a` and `view` are two 8-byte stack
variables, both holding B's address. `free(a)` ends B's lifetime. `a = NULL` changes the
variable `a` only. `view` still holds B's address, and `view[2]` reads freed memory.

**Rejected reading:** "Setting `a = NULL` protects us." It protects *uses of `a`*. The bug
is through `view`. Without ASan, current glibc prints garbage: it writes its
own bookkeeping into the first 16 bytes of a freed small block (a free-list link, then a
tcache check key at offset 8), and `view[2]` sits at byte offset 8, inside that key. gcc
also warns with `-Wuse-after-free`. Whatever it prints is not "the answer". Under ASan it stops with `heap-use-after-free` and points at three stacks:
the read, the `free`, and the `malloc`. M02b reads that report line by line.

## Worked example 2: freeing what you did not allocate

```c
int nums[4] = {1, 2, 3, 4};
int *p = malloc(sizeof nums);
if (p == NULL) exit(EXIT_OOM);
memcpy(p, nums, sizeof nums);
p++;               /* skip the first element */
/* ... */
free(p);           /* BUG */
```

`p` was advanced by one `int`, so it holds B's address plus 4. glibc's `free` first
checks the pointer's alignment: chunks are 16-byte aligned, and B + 4 isn't, so it stops with
`free(): invalid pointer` and aborts before it even looks at a header. Other allocators may
not check, and corrupt themselves instead.

**Rejected reading:** "free works on any address inside the block." It only accepts the exact
value `malloc` returned. If you need to walk a block, walk with a second pointer
(`int *it = p;`) and free the original.

## Patterns to adopt now

- One allocation, one owner, one free. When you write the `malloc`, write the `free` and
  decide where it goes.
- Keep `malloc` and its NULL check adjacent, with nothing in between.
- In a function with several exits, free on every exit. M06 gives you `goto cleanup` for this.
- `free(p); p = NULL;` when `p` stays in scope. Skip it when `p` is about to go out of scope anyway.

## man pointers

- `man 3 malloc`: RETURN VALUE (what NULL means, and what `malloc(0)` may return), NOTES
  (overcommit, and `MALLOC_CHECK_`/`MALLOC_PERTURB_`), and ERRORS (`ENOMEM`). e02 is built
  around this page.
- `man 3 free`: on the same page. Read the sentence about what happens if `ptr` was already freed.
- `man 3 mallopt`: skim `M_PERTURB` only. It fills blocks with a byte pattern, but on current
  glibc small blocks recycled through the per-thread cache (tcache) bypass it, so it is no
  substitute for the tools in M02b. `lesson-demo/` shows it live: a reused 64-byte block keeps
  its old `11` bytes under `MALLOC_PERTURB_=170`, while a reused 4096-byte block comes back as
  `55` (see `lesson-demo/capture-perturb.txt`).

## Check yourself

1. `char *s = malloc(10);` What can you say about `s[0]` before you write it?
2. Why is `sizeof *p` safe to write when `p` is not yet initialised?
3. Two pointers hold the same heap address. You free through one and NULL it. What is the
   state of the block, the first pointer, and the second pointer?
4. Is `free(NULL)` a bug, a no-op, or undefined? Is `free(p)` twice, with `p` unchanged in
   between?
5. Why can a heap overflow crash in a later, unrelated `malloc` instead of at the bad write?

<details><summary>Answers</summary>

1. Nothing. Its value is indeterminate, and reading it before writing is a bug.
2. `sizeof` doesn't evaluate its operand (unless it's a VLA). It only uses the type of `*p`.
3. The block is freed. The first pointer is NULL. The second is dangling: it holds the old
   address, and any use of it is undefined.
4. `free(NULL)` is a defined no-op. A second `free(p)` with `p` unchanged is undefined (a double free).
5. The bad write corrupts allocator metadata or a neighbouring block silently. The damage is
   only noticed when the allocator next reads that metadata, or when the neighbour is used.

</details>

## Takeaways

- `T *p = malloc(n * sizeof *p);`, then check for NULL immediately.
- New blocks are uninitialised. `calloc` (M03) is the exception.
- `free` ends the block's lifetime for every pointer to it, not just the one you passed.
- `free` only accepts what the allocator returned, once. `free(NULL)` is always fine.
- NULLing after free is cheap insurance for one variable, not a substitute for ownership.

## Exercises

| # | Folder | Tag | Size | Topic |
|---|---|---|---|---|
| e01 | `e01-heap-squares/` | `[WRITE]` | S | Allocate, fill, and return an array of `n` squares, with NULL checks and `sizeof *p` |
| e02 | `e02-lookup-malloc/` | `[LOOKUP]` | S | Answer questions from `man 3 malloc` RETURN VALUE and NOTES, then confirm with a probe program |
| e03 | `e03-use-after-free/` | `[BUG]` | S | A stale second pointer reads a freed block; fix it and explain it |
| e04 | `e04-predict-free/` | `[PREDICT]` | S | Six `free` scenarios: no-op, fine, glibc abort, or silent UB? |
| e05 | `e05-dup-array/` | `[WRITE]` | S | Duplicate a borrowed `int` array into a new heap block the caller owns |
| e06 | `e06-trace-free/` | `[TRACE]` | S | Draw pointers and heap blocks through a sequence of mallocs, copies, and frees |

`[LOOKUP]` is reading reference documentation with a purpose. It's graded by `ANSWER.md`
and a small buildable probe, not by checks you make pass.
