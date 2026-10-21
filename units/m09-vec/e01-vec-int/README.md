# m09-e01-vec-int `[WRITE]` (M)

## Goal

Implement a growable array of `int`. `vec.h` declares the type and the API:

```c
typedef struct { int *data; size_t len; size_t cap; } VecInt;

bool vec_push(VecInt *v, int x);
bool vec_pop(VecInt *v, int *out);
bool vec_get(const VecInt *v, size_t i, int *out);
void vec_destroy(VecInt *v);
```

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- `VecInt v = {0};` is a valid empty vector. There is no init function.
- Growth is geometric. The harness pushes 200,000 elements and checks **every** growth: the new
  capacity must be at least `old + old / 4` (a factor of 1.25 or more). Growing by a constant fails it.
- **A failed `vec_push` leaves `*v` exactly as it was** (same `data`, `len`, `cap`) and returns false.
- Capacity arithmetic must not overflow, neither the element count nor the byte count. Fail the push instead.
- `vec_pop` and `vec_get` return false (and leave `*out` alone) when there is nothing to return.
- `vec_destroy` frees the buffer and leaves `*v` as a valid empty vector, so destroying twice is harmless.
- No `realloc(p, 0)`.

## Verify

```bash
make check
```

The harness prints `m09-e01-vec-int: all checks passed`; ASan and Valgrind are silent.
