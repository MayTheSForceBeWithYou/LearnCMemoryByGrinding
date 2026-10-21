# M01 e04 notes

- `pos_static`: the static buffer is sized for the worst case (`INT_MIN` twice is 27 bytes with the NUL), so
  `snprintf` never truncates. INT_MIN works because `%d` formats it directly. Hand-rolled
  digit loops that negate first (`-x`) overflow at INT_MIN, which is signed overflow and so UB, and UBSan
  traps it.
- `pos_into`: `snprintf`'s return value is the length it *wanted*. `n >= cap` means truncated. The
  exact-fit case (`n == cap - 1`) is a fit. Getting `>` vs `>=` wrong is the most common slip.
  `snprintf(dst, 0, ...)` would also write nothing and is legal. The early
  return just makes the contract explicit.
- `pos_heap`: measure with `snprintf(NULL, 0, ...)`, allocate `n + 1`, then format. M07 drills this.
- **What to ship:** `pos_into` for a library, because it can't leak, has no OOM path, and is reentrant. Then maybe
  `pos_heap` as a convenience on top. `pos_static` is acceptable only for quick debug printing.
