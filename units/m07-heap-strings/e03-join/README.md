# M07 e03: Join with exact sizing `[WRITE]`

## Goal

Implement

```c
char *str_join(const char *const *parts, size_t n, const char *sep);
```

`str_join` returns a new string with the `n` parts separated by `sep`: `{"a","b","c"}` with `", "`
gives `"a, b, c"`. If `n == 0`, it returns an empty heap string `""` (not NULL). It returns NULL on
allocation failure or if the total size would overflow `size_t`.

**The starter will not build until you write the definition.** The link error is intentional.

## Constraints

- Exactly one allocation, of exactly the right size. No `realloc`, no `strcat`.
- Compute each part's length once. The harness joins 10000 parts, and a quadratic join is too slow
  under ASan to finish in the time limit.
- `parts` and `sep` are borrowed. The result is owned by the caller.

## How to verify

- `make check`.
