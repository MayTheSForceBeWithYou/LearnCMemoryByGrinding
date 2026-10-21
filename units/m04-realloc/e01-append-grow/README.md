# M04 e01: Push with `realloc` `[WRITE]`

## Goal

Implement a minimal growable `int` array:

```c
struct ints { int *data; size_t len, cap; };

int  ints_push(struct ints *v, int x);    /* 0 on success, -1 on OOM */
void ints_free(struct ints *v);           /* frees, leaves *v empty and reusable */
```

A zero-initialised `struct ints` (`{0}`) is a valid empty array. The harness pushes a
hundred thousand values, checks them, frees, reuses the same struct, and checks again.

**The starter will not build until you write both definitions.** The link error is intentional.

## Constraints

- The first push must work through `realloc(NULL, ...)`, with no separate `malloc` branch.
- Growth is geometric, and the capacity computation must not overflow (return -1 if it would).
- On failure, `*v` is unchanged: same `data`, same `len`, same `cap`.
- After `ints_free(v)`, `v->data == NULL` and both counts are 0.
- Don't call `realloc` with size 0, ever.

## How to verify

- `make check`: checks, ASan, and Valgrind clean.
- This exercise isn't `[FAULT]`-graded, but your code will be reused in e06, which is. You can
  run `make fault` now to see how far you get.
