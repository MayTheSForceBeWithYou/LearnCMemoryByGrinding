# M02 e01 notes

- `malloc(n * sizeof *v)`: if someone changes `long` to `int64_t` later, this stays right.
- **n == 0.** `malloc(0)` may return NULL *or* a unique pointer (glibc returns a minimum-size block). Both
  are valid for `free`. But a NULL from `malloc(0)` is indistinguishable from failure, and `main` would
  report OOM. Returning NULL before calling `malloc` makes the contract explicit. The alternative (always call
  `malloc`) is fine if the caller never treats NULL with `n == 0` as an error.
- `sum_and_free(NULL, 0)`: the loop body never runs, and `free(NULL)` is a no-op. No special case needed.
- **Common mistake:** `if (v) free(v);`, which is redundant. Or computing `i * i` in `size_t` and casting afterwards,
  which works here but hides the type of the arithmetic.
- The OOM path in `main` returns `EXIT_OOM` with nothing live, because every earlier block was already passed to `sum_and_free`.
