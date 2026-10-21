# M03 e03 notes

- `b != 0 && a > SIZE_MAX / b` is exact. `SIZE_MAX / b` is the largest `a` whose product fits, because division rounds toward zero.
  The `b != 0` guard comes first, both to avoid division by zero and because nothing times 0 overflows.
- Common wrong versions: `a * b / b != a` (computes the wrapped product first, which is correct but obscure and divides by zero when b == 0),
  `a * b < a` (misses most overflows), and `a >= SIZE_MAX / b` (rejects the exact boundary, which the harness tests).
- `alloc_array0` reuses `alloc_array`, so the overflow decision lives in one place. `memset` with the checked product is safe.
- `malloc(0)` may return NULL or a unique pointer. Both are fine to `free`, which is all the harness does.
- On overflow we never call `malloc`, so ASan's `allocation-size-too-big` abort never triggers. That's the point of checking first.
