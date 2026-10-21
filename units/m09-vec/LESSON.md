# M09: The dynamic array

## Organizer

M04 taught `realloc` one call at a time. This unit packages it into the most used data
structure in C programs that have outgrown fixed arrays: a growable array, here called `Vec`.
It is small enough to write in an hour and has enough sharp edges to keep you busy for a week.

You need M04 (the `tmp` pattern, failed-realloc leaks, interior pointers), M05 (ownership
contracts, `destroy` accepting NULL) and M07 (heap strings) for e05.

The unit covers:

- the `len`/`cap` invariant and why both numbers exist;
- push, pop, insert and remove, with growth that is cheap on average;
- shrink-to-fit, and why its failure is not an error;
- vectors that **own** their elements (a `Vec` of `char *`);
- the pointer into `v.data` that dies on the next push.

## Foundations

### The struct and its invariant

```c
typedef struct {
    int    *data;   /* NULL when cap == 0 */
    size_t  len;    /* elements in use */
    size_t  cap;    /* elements allocated */
} VecInt;
```

The invariant every function keeps: `len <= cap`, `data` points to at least `cap` elements
(or is NULL when `cap == 0`), and elements `[0, len)` (that is, `data[i]` for `0 <= i && i < len`) are initialised. Elements
`[len, cap)` (`len <= i && i < cap`) exist
but are not part of the vector. Reading them is a logic bug, and if they were never written,
Valgrind calls it an uninitialised read.

A zeroed struct (`VecInt v = {0};`) is a valid empty vector. Design for that: it makes
initialisation impossible to forget, and `realloc(NULL, n)` behaves as `malloc(n)`, so the first
push needs no special case.

### Growth: why doubling

Grow by one element per push and pushing `n` elements copies `1 + 2 + ... + (n-1)` elements in
the worst case, which is quadratic. Grow by a factor (×2 here; ×1.5 is also common) and the
total copying across `n` pushes is fewer than `2n` element copies, because each growth copies the old capacity,
the old capacities sum to less than the final one, and the final one is less than `2n`. So each push costs O(1) *amortized*.
Most pushes are a store and an increment, and the occasional one pays for a big copy.

The new capacity is arithmetic on `size_t`, so it can overflow:

- `cap * 2` overflows when `cap > SIZE_MAX / 2`;
- `new_cap * sizeof *v->data` overflows long before that.

Check both, or compute bytes with `reallocarray`, which returns NULL with `ENOMEM` instead of wrapping. Under ASan
that holds only with `allocator_may_return_null=1` (set in the course's `ASAN_OPTIONS`); with ASan's defaults the
oversized request aborts with a report instead of returning NULL.

### Growth must not lose the vector

The M04 rule, restated as a vector guarantee: **if push fails, the vector is exactly as it
was.** Same `data`, same `len`, same `cap`, every element intact. That means `realloc` into a
temporary, and update `data` and `cap` only after it succeeds. Callers can then report the
failure and keep going, or clean up with the ordinary destroy function.

### Insert and remove

Inserting at index `i` shifts `[i, len)` one slot right. Removing shifts `[i+1, len)` one slot left.
The ranges overlap, so the tool is `memmove`, not `memcpy`. Valid indices differ:
insert accepts `i == len` (append), remove does not.

### Shrink-to-fit

`realloc` to a smaller size usually succeeds in place, but the standard lets it fail and
return NULL. When shrinking fails, the old block is still valid and still big enough.
Treat failure as "nothing happened": return the old vector unchanged, and don't report OOM.
Shrinking to `len == 0` deserves a decision: `free` and set `data = NULL, cap = 0`. Do not call
`realloc(p, 0)` (M04: implementation-defined in C17, undefined in C23).

### Element ownership

A `Vec` of `int` owns its buffer only. A `Vec` of `char *` raises the M05 question for every
element: does the vector own the strings? If it does, then:

- `destroy` frees every element, then the buffer;
- `pop` must **transfer** the string to the caller (who now frees it), or free it itself.
  Say which;
- `push` either copies its argument (and can fail in two places: the copy and the growth) or
  takes ownership of it. If it takes ownership and growth fails, who frees the argument?
  The contract must say.

### Pointers into the buffer

`&v.data[i]` is an interior pointer into a block that `realloc` may move. Any push that grows the
vector can invalidate every such pointer, and so can shrink-to-fit. The bug is real on every build,
but it only *shows* when the block actually moves, and plain glibc often resizes in place. On the
course machine, `lesson-demo/` (which grows a 64-byte block to 128 bytes and then shrinks it to 32, 100
times) saw plain glibc move the block 0 times out of 100 in both directions (`capture-run.txt`). ASan's `realloc` moved it
100 times out of 100 (`capture-asan.txt`), because ASan's `realloc` always returns a new block and frees the old one. That is
why ASan grades this bug: under ASan, *every* `realloc` that grows the buffer makes every old pointer into it
stale, and the next use is reliably a use-after-free. Only a resizing `realloc` causes it. Pushes into
spare capacity move nothing. Under the plain
build, a test that "works" proves nothing. The robust habits: hold **indices**, not pointers, across anything that may grow the vector.
Or reserve capacity up front and document that pointers stay valid until `len` reaches `cap`.

## Worked example: a reserve that keeps its promise

`reserve` is a neighbour of the exercises. It ensures room for at least `extra` more elements.

```c
#define _DEFAULT_SOURCE            /* reallocarray() under -std=c17; before any #include */
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Ensures v->cap >= v->len + extra. On failure returns false and leaves *v unchanged. */
bool vec_reserve(VecInt *v, size_t extra)
{
    if (extra > SIZE_MAX - v->len)
        return false;                         /* len + extra would wrap */
    size_t need = v->len + extra;
    if (need <= v->cap)
        return true;

    size_t new_cap = v->cap ? v->cap : 8;
    while (new_cap < need) {
        if (new_cap > SIZE_MAX / 2) { new_cap = need; break; }
        new_cap *= 2;
    }

    int *tmp = reallocarray(v->data, new_cap, sizeof *v->data);  /* glibc >= 2.26 */
    if (!tmp)
        return false;                         /* v->data still valid, still owned by v */
    v->data = tmp;
    v->cap  = new_cap;
    return true;
}
```

**The plausible wrong reading.** "I wrote `v->data = realloc(v->data, ...)` and checked the
result for NULL, so I handle failure." You detect it, but on failure you have already overwritten
your only pointer to the old block with NULL. That leaks the block and destroys the vector's
contents. The checked-but-wrong version passes every test where memory never runs out, which is
every test until the fault sweep. Under `FAIL_AT` it shows up as a leak report naming the
*previous* successful allocation, which is confusing until you see why.

A second wrong reading, about held pointers: "My ASan run was clean with `int *p = &v.data[0];`
held across 3 pushes, so the pattern is safe here." Three pushes from a fresh vector with
initial capacity 8 never grow it. Nothing moved because nothing grew. Push a ninth and
the same code becomes a heap-use-after-free under ASan. e04 has the report.

## `man` pointers

- `man 3 realloc` (same page as `malloc`): RETURN VALUE, especially the failure case; `reallocarray`.
- `man 3 memmove`: why overlap is allowed here and not with `memcpy`.
- `man 7 feature_test_macros`: what `_DEFAULT_SOURCE` turns on with `-std=c17`.
- `man 3 malloc` again: `reallocarray` fails with `ENOMEM` when `nmemb * size` would overflow (ERRORS).

## Check yourself

1. Starting from `{0}` with initial capacity 8 and doubling, how many times does `realloc` run
   while pushing 100 elements? What is the final `cap`?
2. Why does `insert` accept `i == len` but `remove` does not?
3. Shrink-to-fit's `realloc` returns NULL. Is the vector still usable? Did you leak?
4. A `Vec` of owned `char *` has `pop` return the string. After `pop`, whose job is `free`?
5. Why is "the ASan run was clean" not evidence that holding `&v.data[i]` across a push is safe?

<details><summary>Answers</summary>

1. Capacities 8, 16, 32, 64, 128: five calls (the first acts as `malloc`). Final `cap` is 128.
2. Inserting at `len` appends. Removing at `len` would remove an element that doesn't exist.
3. Usable: yes. Leaked: no. The old block is untouched and still owned by the vector.
4. The caller's. `pop` transferred ownership, and the vector has forgotten the pointer.
5. ASan only reports accesses to memory that actually was freed. If no push grew the vector on
   that run, there was no `realloc`, so nothing was freed and there was nothing to report. (Once a
   push does grow it, ASan's `realloc` always moves the block, so the report is reliable from then on.)

</details>

## Takeaways

- Keep `len <= cap` and `data[0..len-1]` initialised after every call, including failed ones.
- Grow geometrically and check the arithmetic. `reallocarray` does the byte-size check for you.
- A failed push leaves the vector unchanged. That is the whole point of the `tmp` pattern.
- A failed shrink is not an error.
- Owning vectors need a contract for push (copy or take), pop (transfer) and destroy (elements, then buffer).
- Interior pointers die on growth. Hold indices.

## Exercises

| # | Folder | Tag | Size | What |
|---|---|---|---|---|
| 1 | `e01-vec-int` | `[WRITE]` | M | `VecInt`: push, pop, get, destroy, amortized growth |
| 2 | `e02-insert-remove` | `[WRITE]` | S | Insert and remove at an index with `memmove` |
| 3 | `e03-growth-count` | `[PREDICT]` | | Predict `FA_COUNT` for doubling vs grow-by-one |
| 4 | `e04-held-pointer` | `[REPORT]` | | Pinpoint the dangling `&v.data[i]` from an ASan report |
| 5 | `e05-strvec` | `[DESIGN]` `[FAULT]` | M | A vector that owns its strings: contract first, then code |
| 6 | `e06-vec-under-fire` | `[FAULT]` | | Push, reserve and shrink-to-fit that survive every `FAIL_AT` |
