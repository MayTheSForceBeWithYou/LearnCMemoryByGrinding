# m09-e02-insert-remove `[WRITE]` (S)

## Goal

Add positional insert and remove to the `VecInt` from e01. The starter ships working
push, pop, get and destroy in `main.c`. You add:

```c
bool vec_insert(VecInt *v, size_t i, int x);   /* valid: i <= len */
bool vec_remove(VecInt *v, size_t i, int *out); /* valid: i <  len; out may be NULL */
```

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- Order of the other elements is preserved.
- Out-of-range indices return false and change nothing.
- A failed insert (growth failed) leaves the vector unchanged.
- Shift with one `memmove` per call, not an element-by-element loop.

## Verify

```bash
make check
```

The harness prints `m09-e02-insert-remove: all checks passed`. It includes insert at 0, at
`len`, into an empty vector, and remove of the last element. An off-by-one in the `memmove` length
*may* show up in `make asan` as a heap-buffer-overflow, but only if it runs past `cap`. One that stays
inside spare capacity is invisible to ASan, and the content checks are there for that case.
