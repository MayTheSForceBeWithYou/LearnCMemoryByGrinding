# M03 e03: A checked array allocator `[WRITE]`

## Goal

Implement

```c
void *alloc_array(size_t n, size_t elem);      /* uninitialised, like malloc */
void *alloc_array0(size_t n, size_t elem);     /* zeroed, like calloc */
```

Both return NULL and set `errno = ENOMEM` if `n * elem` would overflow `size_t`, and
otherwise behave like `malloc(n * elem)` and `calloc(n, elem)`. The harness tests the
exact boundary: the largest `n` that is accepted and the smallest that is rejected, for
several values of `elem`, including 1, 3, 8, and `SIZE_MAX`. It doesn't try to allocate the
huge sizes. It checks the overflow decision through a pure helper you also write:

```c
bool mul_overflows(size_t a, size_t b);
```

**The starter will not build until you write the three definitions.** The link error is intentional.

## Constraints

- No wider integer types (`unsigned __int128`) and no `__builtin_mul_overflow`. Write the
  check with division, before the multiplication.
- `alloc_array0` must not call `calloc`. Build it from the other two functions.
- `n == 0` or `elem == 0` is not an overflow. Both functions then return whatever
  `malloc(0)` returns, and the harness frees it.

## How to verify

- `make check`: checks, ASan, and Valgrind clean.
