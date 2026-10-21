# m09-e06-vec-under-fire `[FAULT]`

## Goal

Implement the growth-related operations of `VecInt` so that they survive a failure at every
allocation point. The rest of the vector ships complete in `main.c`.

```c
bool vec_reserve(VecInt *v, size_t extra);
bool vec_push(VecInt *v, int x);
void vec_shrink_to_fit(VecInt *v);
```

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- `vec_reserve` and `vec_push`: on failure return false and leave `*v` unchanged.
- `vec_shrink_to_fit` never fails from the caller's point of view. If shrinking fails, the vector
  keeps its current buffer. When `len == 0` it frees the buffer and leaves `{NULL, 0, 0}`.
  No `realloc(p, 0)`.
- The harness keeps working after a failed push (it checks contents are intact), then exits
  `EXIT_OOM` (3) with a one-line stderr message. That part is written.

## Verify

```bash
make fault
make check
```

Every `FAIL_AT` run either prints `m09-e06-vec-under-fire: all checks passed` or exits 3,
with no leak and no sanitizer report.

<details><summary>Hint</summary>

Some `FAIL_AT` values land on a shrink. Those runs should *pass*, not exit 3. If one exits 3,
your shrink is reporting a failure the contract says it must absorb.

</details>
