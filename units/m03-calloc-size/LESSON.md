# M03: `calloc`, zeroing, and size arithmetic

## Advance organizer

M02's idiom `malloc(n * sizeof *p)` hides a multiplication, and multiplications can
overflow. When `n * sizeof *p` wraps around, `malloc` succeeds with a *small* block, and
the loop that fills `n` elements writes far past its end. This is one of the most common
real-world heap vulnerabilities. This unit covers:

- what `calloc` guarantees (zeroed memory, **and** an overflow-checked multiplication);
- how unsigned `size_t` arithmetic wraps, and how a negative `int` becomes a huge `size_t`;
- the `SIZE_MAX / n` pre-check, and `reallocarray`, which does it for you.

## Foundations

### `size_t` arithmetic wraps, and that is defined behaviour

`size_t` is unsigned, 64 bits on x86-64. Unsigned arithmetic in C is modulo 2^64: a result
that doesn't fit is reduced, silently, with no trap and no UB. So

```c
size_t n = (SIZE_MAX / 8) + 2;          /* about 2.3e18 elements */
double *v = malloc(n * sizeof *v);      /* n * 8 wraps to 8 */
```

asks for **8 bytes**. `malloc` happily returns a block of 8 bytes. Every write to `v[1]`
and beyond is a heap buffer overflow. No sanitizer complains about the multiplication,
because it's legal. ASan catches the *first out-of-bounds write*, if the test reaches it.

Signed overflow, by contrast, is UB, and UBSan (`-fsanitize=undefined`, part of the course's ASan build) does trap it. But size
computations are `size_t`, so UBSan is no help with them.

### The overflow check

`a * b` overflows a `size_t` exactly when `b != 0 && a > SIZE_MAX / b`. Integer division
rounds down, so `SIZE_MAX / b` is the largest `a` for which `a * b <= SIZE_MAX`. In C:

```c
#include <stdint.h>   /* SIZE_MAX */

void *alloc_array(size_t n, size_t elem) {
    if (elem != 0 && n > SIZE_MAX / elem) {
        errno = ENOMEM;
        return NULL;
    }
    return malloc(n * elem);
}
```

This check belongs *before* the multiplication. Checking the product afterwards (for example,
"is `n * elem < n`?") is wrong for most sizes: a wrapped product can still be larger than `n`.

### `calloc(n, size)`

- Performs the same overflow check internally and returns NULL (with `errno` set to `ENOMEM`)
  if `n * size` doesn't fit.
- Returns memory where **every byte is zero**. For integers and `char`, that's the value 0.
  For pointers and floating point, all-bits-zero is NULL and 0.0 on every platform you'll
  meet, though the standard doesn't promise it.
- Can be *cheaper* than `malloc` plus `memset` for large blocks: memory fresh from the kernel
  (`mmap`) is already zero, and glibc knows not to zero it twice.

Use `calloc` when you'll rely on the zeroes, and when you want the overflow check anyway.
Use `malloc` when you'll overwrite every byte immediately.

**`calloc` does not zero memory obtained later from `realloc`.** The grown tail is
uninitialised. M04 comes back to this.

### Impossible sizes, and sizes that wrap

There are two failure modes. The first is a size no allocator can provide: `malloc(SIZE_MAX)`, or
`calloc(n, size)` where `n * size` overflows. The standard answer is NULL with `errno` set to
`ENOMEM`. By default ASan reports `allocation-size-too-big` or `calloc-overflow` and aborts
instead. The course runs ASan with `allocator_may_return_null=1`, which restores the standard
behaviour, so your NULL checks are exercised in every build.

The second is worse: a product that **wraps to a small number**. On 64-bit,
`(SIZE_MAX / 8 + 2) * 8` is 8. Pass that to `malloc` unchecked and it succeeds; you now hold 8
bytes and believe you hold 2^64. No allocator and no sanitizer can catch this at the call,
because the call itself is legal. Only your check *before* the multiplication can. The
exercises test both modes.

### `reallocarray(p, n, size)`

`reallocarray(p, n, size)` is `realloc(p, n * size)` with the overflow check. It's in glibc (since 2.26), the
BSDs, and POSIX.1-2024. `reallocarray(NULL, n, size)` is a checked `malloc` that doesn't
zero. The course's fault injector wraps it too, so it is fair game in `[FAULT]` exercises.
It needs `_DEFAULT_SOURCE` (or `_GNU_SOURCE`) defined before including `<stdlib.h>` under
`-std=c17`.

### `int` counts and negative sizes

```c
int count = read_count_from_file();     /* returns -1 on a corrupt header */
struct rec *r = malloc(count * sizeof *r);
```

`count * sizeof *r` converts `count` to `size_t` first (the usual arithmetic conversions).
`-1` becomes `SIZE_MAX`, and the product wraps to `SIZE_MAX * 24 mod 2^64`, which is
`2^64 - 24`. That's huge, so `malloc` fails, and you got lucky. Now change one character of
the code:

```c
struct rec *r = malloc((count + 1) * sizeof *r);   /* +1 for a sentinel record */
```

`count + 1` is computed in `int`: `-1 + 1` is `0`. `malloc(0)` is implementation-defined: it returns NULL or a unique pointer you may not dereference. glibc returns a small block, and
a loop like `for (int i = 0; i != count; i++)` (or one that trusts a second field of the header)
writes records into it. Rule: **validate counts in their signed form**, before any arithmetic
or conversion, and keep sizes as `size_t` from then on.

Spelled out in C, the valid range is `count >= 0 && count <= MAX_RECS`. Don't write
`0 <= count <= MAX_RECS`, which C parses as `(0 <= count) <= MAX_RECS`, so it compares 0 or 1 with `MAX_RECS`.

## Worked example 1: the image header

```c
struct image { uint32_t w, h; uint32_t *px; };

int image_alloc(struct image *im, uint32_t w, uint32_t h) {
    im->px = malloc(w * h * sizeof *im->px);
    return im->px ? 0 : -1;
}
```

With `w = h = 65536`, the product `w * h` is computed in `uint32_t` (both operands are
`uint32_t`, and `int` is 32 bits, so there's no promotion to a wider type). 2^32 mod 2^32 is 0,
so we request `0 * 4 = 0` bytes. glibc's `malloc(0)` returns a small block, and the pixel loop
writes 16 GiB into it.

**Rejected reading:** "Multiplying by `sizeof` makes it `size_t`, so it's fine." `*` groups left to
right, so the expression is `(w * h) * sizeof *im->px`. This is grouping, not evaluation order. The damage is done in the `uint32_t`
multiplication before `size_t` enters. The fix converts first and checks both steps:

```c
size_t npx;
if (h != 0 && (size_t)w > SIZE_MAX / h) return -1;
npx = (size_t)w * h;
im->px = calloc(npx, sizeof *im->px);   /* calloc checks the second multiply */
```

## Worked example 2: `malloc` plus `memset` versus `calloc`

```c
int *a = malloc(n * sizeof *a);
if (a) memset(a, 0, n * sizeof *a);

int *b = calloc(n, sizeof *b);
```

Both give zeroed memory when `n` is sane. They differ when `n * sizeof *a` overflows: `a`
gets a tiny block, and `memset` zeroes the tiny block correctly (the same wrapped size). The
caller thinks it has `n` zeroed ints. `b` is NULL.

**Rejected reading:** "`memset` would crash on overflow, so we'd notice." `memset` is passed the
same wrapped count, so it does exactly what it was told and nothing visibly fails until much later.

## man pointers

- `man 3 calloc`: same page as `malloc`. Read what it says about overflow and `ENOMEM`.
- `man 3 reallocarray`: same page. Look for the feature-test macro in SYNOPSIS.
- `man 7 feature_test_macros`: what `_DEFAULT_SOURCE` does and why `-std=c17` hides `reallocarray`.
- `man 0p stdint.h` (if `man-pages-posix` is installed): `SIZE_MAX`.

## Check yourself

1. Write the C condition for "`a * b` would overflow `size_t`" without performing the multiplication.
2. Why is checking `if (n * elem < n)` after the multiply not a correct overflow test?
3. An `int count` comes from user input. Write the C range check that must pass before you
   compute a size from it, with `MAX` as the upper bound (inclusive).
4. Which of `calloc`, `malloc`, and `reallocarray` both check overflow and zero memory?
5. In `w * h * 4` with `uint32_t w, h`, at which step does wrapping happen, and in which type?

<details><summary>Answers</summary>

1. `b != 0 && a > SIZE_MAX / b`.
2. A wrapped product is the true product minus a multiple of 2^64, and it can still be
   `>= n`. Example: `n = 2^63`, `elem = 3` wraps to `2^63`, which is not `< n`, so the test
   passes a wrapped size. It is reliable only for `elem == 2`. Check before multiplying.
3. `count >= 0 && count <= MAX`, checked while `count` is still an `int`.
4. Only `calloc` does both. `reallocarray` checks overflow but doesn't zero; `malloc` does neither.
5. At `w * h`, in `uint32_t`, because `*` groups left to right and both operands are
   `uint32_t`. The `* 4` happens afterwards on the already-wrapped value.

</details>

## Takeaways

- `size_t` overflow is silent and defined, and it produces an *undersized* successful allocation.
- Check `elem != 0 && n > SIZE_MAX / elem` before multiplying, or let `calloc` or `reallocarray` do it.
- Convert to `size_t` before the first multiply. Order of evaluation matters.
- Validate signed counts as signed (`c >= 0 && c <= MAX`) before converting.
- `calloc` zeroes. `realloc`'s grown tail is never zeroed.

## Exercises

| # | Folder | Tag | Size | Topic |
|---|---|---|---|---|
| e01 | `e01-zeroed-tally/` | `[WRITE]` | S | A histogram that relies on zeroed counters: `calloc` versus `malloc` plus `memset`, graded by Valgrind |
| e02 | `e02-break-count/` | `[BREAK]` | S | Find a count that makes naive `malloc(n * size)` code allocate too little, and prove it with ASan |
| e03 | `e03-checked-array/` | `[WRITE]` | S | `alloc_array(n, elem)` with a correct `SIZE_MAX` pre-check, tested at the boundary |
| e04 | `e04-predict-conversions/` | `[PREDICT]` | S | Predict the `size_t` value of ten mixed `int`, `unsigned`, and `size_t` expressions |
| e05 | `e05-negative-count/` | `[BUG]` | S | A record loader trusts an `int` count from a header; fix the validation |

`[BREAK]` is `[BUG]`-family in spirit (its Makefile says `TAG = PREDICT`, because the
starter must build and run cleanly for normal input): the program is complete and buildable, and your deliverable is an
input that breaks it, plus `ANSWER.md`, plus a fix.
