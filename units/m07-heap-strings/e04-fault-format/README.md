# M07 e04: Formatted strings that survive failure `[FAULT]`

## Goal

Implement

```c
char *fmt_alloc(const char *fmt, ...);            /* printf-style; caller frees; NULL on failure */
char *report_build(const struct player *ps, size_t n);
```

`fmt_alloc` measures with `vsnprintf(NULL, 0, ...)`, allocates exactly, and formats. `report_build`
returns one heap string with one line per player (`"Ada: 12/20 HP\n"`, and so on), built from `fmt_alloc`
pieces that you then concatenate (or measure and write directly: your choice). The harness checks
the exact output.

**The starter will not build until you write both definitions.** The link error is intentional.

## Constraints

- `fmt_alloc` must use `va_copy`. Traversing one `va_list` twice is undefined.
- `report_build` must not leak on any `FAIL_AT`, including failures halfway through the list of
  players, where some per-line strings already exist.
- No `asprintf`. It allocates inside libc, so the sweep couldn't test it.

## How to verify

- `make fault` and `make check`.
