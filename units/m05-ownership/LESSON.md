# M05: Ownership and create/destroy pairs

## Advance organizer

C has no ownership in its type system. `char *` looks the same whether the function giving
it to you expects you to free it, expects you *not* to, or will free it behind your back on
the next call. **Ownership is a contract you write in comments and enforce by discipline.**
Every leak and every double free in M01-M04 was a contract nobody wrote down.

This unit gives you a vocabulary and a set of conventions you'll use for the rest of the
course:

- **owner:** the one party responsible for eventually freeing a block. Exactly one at a time.
- **borrow:** temporary access with no right to free, valid only for a stated period.
- **transfer (move):** ownership passes from one party to another. The giver must stop using it.
- **create/destroy pair:** every type that owns heap memory gets one constructor and one destructor,
  and they are the only places that allocate and free its internals.

`LearnCwithRPG-terminal` ch10 introduced `map_create`/`map_destroy`. This unit makes the
contract explicit and covers the cases ch10 didn't: out-parameters, transfer, opaque types,
and NULL-tolerant destroy.

## Foundations

### Writing the contract

Put it in the header, on the declaration, in a form a reader can check against a call site:

```c
/* Returns a new player named `name` (copied; caller keeps `name`).
 * Ownership: caller owns the result and must pass it to player_destroy().
 * Returns NULL on allocation failure. */
struct player *player_create(const char *name);

/* Frees `p` and everything it owns. Accepts NULL (no-op). */
void player_destroy(struct player *p);

/* Borrowed: valid until `p` is destroyed or player_rename() is called on it. */
const char *player_name(const struct player *p);
```

Three questions each comment answers: **who frees the result, what happens to the
arguments, and how long a borrowed pointer stays valid.** `const` on a returned pointer is a
useful hint ("don't modify, and so don't free"), but it is not enforcement. `free` takes
`void *`, and the compiler lets a cast through.

### Borrowing

A function parameter `const T *` is borrowed by default: the callee reads it during the
call and keeps no copy. If the callee *does* keep the pointer (stores it in a struct), that's
a different contract and must be documented: "`name` must outlive the returned object."
That kind of stored borrow is legal, but it's where dangling pointers come from, because the
caller has to keep a promise the compiler doesn't see.

### Transfer

```c
/* Takes ownership of `buf` (must come from malloc). Do not use or free it after the call,
 * even if this returns -1: on failure, buf has already been freed. */
int log_push_owned(struct log *lg, char *buf);
```

The failure path is part of the contract. "Takes ownership even on failure" and "on
failure, ownership stays with the caller" are both reasonable, but the caller's code is
different for each, so you must say which. This course's convention: **transfer happens on
success only, unless the comment says otherwise.**

### Out-parameters: `T **out`

When the return value is taken by an error code, return the object through a pointer to the
caller's pointer variable:

```c
/* On success returns 0 and stores a new list in *out (caller owns; list_destroy).
 * On failure returns -1 and leaves *out unchanged. */
int list_parse(const char *text, struct list **out);
```

`out` is the address of the caller's `struct list *` variable, which lives in the caller's
frame. Writing `*out = l;` updates that variable. The common mistake is writing `out = &l;`,
which changes only the callee's copy of `out` and leaves the caller's pointer untouched.
Assign to `*out` **only on success**, as the last step, so the caller never sees a
half-built object.

### Destroy accepts NULL, and leaves no traps

`player_destroy(NULL)` does nothing, just like `free(NULL)`. This makes cleanup code
(M06) simple. A destroy can't NULL the caller's pointer (it got a copy). If you want that,
the API is `void player_destroy(struct player **pp)`, which frees `*pp` and sets
`*pp = NULL`. Pick one style per codebase. The course uses the single-pointer form plus
`p = NULL` at the call site.

### Opaque types

```c
/* stack.h */
typedef struct stack stack;           /* incomplete: callers can't see inside */
stack *stack_create(void);
void   stack_destroy(stack *s);
```

The struct definition lives only in `stack.c`. Callers can hold a `stack *` but can't
`malloc(sizeof(stack))` (the size is unknown to them), can't copy the struct by value, and
can't reach in and free a field. Ownership of the internals is guaranteed to stay with
`stack.c`. This is the C way to make a contract enforceable rather than merely documented.

### `strdup` and strict C17: `_POSIX_C_SOURCE`

From this unit on you'll use `strdup` (and, in M07, `strndup`) to copy strings. They aren't
part of C17 (they joined the standard only in C23), but POSIX defines them. The course
compiles with `-std=c17`, which asks glibc's headers to declare **only** ISO C names. Without
help, `strdup` is undeclared: you get an "implicit declaration" error, or with older compilers
a warning plus a call through an implicitly declared `int`-returning function, which truncates the
64-bit pointer. The fix is a **feature-test macro**, defined before the first `#include`:

```c
#define _POSIX_C_SOURCE 200809L   /* POSIX.1-2008: strdup, strndup, getline */
#include <stdlib.h>
#include <string.h>
```

It has to come first, because the headers decide what to declare the first time any of them is
included. `_DEFAULT_SOURCE` (M03's `reallocarray`) is the glibc-specific superset. `man 7 feature_test_macros`
has the full story. Every starter that needs `strdup` already has the line. Write it yourself in new files.

## Worked example 1: who frees this?

```c
const char *cfg_get(const struct cfg *c, const char *key);   /* borrowed */
char       *cfg_dup (const struct cfg *c, const char *key);  /* caller frees */

const char *a = cfg_get(c, "name");
char *b = cfg_dup(c, "name");
cfg_reload(c);           /* frees and rebuilds all stored strings */
printf("%s %s\n", a, b);
free(b);
```

In memory, `cfg` owns a block holding `"name"`'s value, call it V1. `a` borrows V1. `b`
owns a separate block, V2, a copy. `cfg_reload` frees V1 and creates V3. `a` now dangles,
and `b` is unaffected.

**Rejected reading:** "Both came from the same key, so both are equally valid." Validity
depends on *who owns the bytes*, not where the text came from. `a`'s lifetime is tied to
`c`'s internal state, and `b`'s lifetime is tied to the `free(b)`.

## Worked example 2: the out-parameter that didn't

```c
int make_buf(size_t n, char **out) {
    char *p = malloc(n);
    if (!p) return -1;
    out = &p;           /* BUG */
    return 0;
}
```

`out` is a local copy of the caller's `&buf`. `out = &p` points that copy at the callee's
local `p`, which dies at return. The caller's `buf` is unchanged (still NULL, or garbage if
uninitialised), and the block `p` pointed to is leaked.

**Rejected reading:** "It's a pointer to a pointer, so assigning it changes the caller's
pointer." Assigning `out` changes `out`. Only assigning `*out` writes through to the
caller's variable. Draw the two frames and the arrows, and the difference is obvious.

## man pointers

- `man 3 getline`: a real out-parameter contract (`char **lineptr`, `size_t *n`). Note how it
  states who allocates, who frees, and what happens when it grows the buffer.
- `man 3 strdup`: the canonical "caller frees the result" function.
- `man 3 getenv`: the canonical "borrowed, do not free, may be invalidated" function.

## Check yourself

1. A header says `const char *name_of(const struct x *)` with no comment. List three
   different contracts it could have.
2. What must a "takes ownership" comment say about the failure path, and why?
3. In `int f(struct t **out)`, why should `*out` be assigned last?
4. What can a caller *not* do with an opaque `stack *` that it could do with a complete type?
5. Why should `destroy(NULL)` be a no-op?

<details><summary>Answers</summary>

1. It returns a pointer into the struct (borrowed, valid until the struct changes or dies);
   a pointer to a static or literal string (borrowed forever); or a new heap copy (owned by
   the caller, who must free it despite the `const`).
2. Whether the callee still takes ownership when it fails. Without that, the caller can't
   know whether to free the argument, and either choice is a leak or a double free.
3. So that on any failure path, the caller's variable is never left pointing at something
   the function has already freed. The caller sees either the finished result or nothing.
4. Declare one by value, use `sizeof`, or read or write its fields. It can only hold
   pointers and pass them to the API.
5. So cleanup code can call it unconditionally on pointers that may never have been
   assigned, the same way `free(NULL)` works.

</details>

## Takeaways

- Every pointer crossing an API boundary needs a contract: owned, borrowed (until when), or transferred.
- Write the contract on the header declaration, including the failure path.
- `T **out`: write `*out`, only on success, last.
- Destroy functions accept NULL and free everything the object owns.
- Opaque types let the compiler enforce part of the contract: no field access, no `sizeof`, no copies by value. They can't stop a double destroy.

## Exercises

| # | Folder | Tag | Size | Topic |
|---|---|---|---|---|
| e01 | `e01-who-frees-this/` | `[TRACE]` | S | For twelve call sites, classify each returned pointer as owned, borrowed, or static, and trace its lifetime |
| e02 | `e02-design-contract/` | `[DESIGN]` | M | Write the ownership contract for a `struct contact` API, then implement it |
| e03 | `e03-out-param/` | `[WRITE]` | S | `parse_pair(const char *, struct pair **out)` with an all-or-nothing out-parameter |
| e04 | `e04-borrowed-freed/` | `[BUG]` | S | A caller frees a borrowed name; an API keeps a borrow that outlives its source |
| e05 | `e05-design-opaque/` | `[DESIGN]` | M | An opaque string stack: header, contract, implementation, NULL-safe destroy |
| e06 | `e06-transfer/` | `[WRITE]` | S | `queue_push_owned` that takes ownership, with a documented failure-path contract |
