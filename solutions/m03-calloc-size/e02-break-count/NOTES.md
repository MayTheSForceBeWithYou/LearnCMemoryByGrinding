# M03 e02 notes

- `n * (uint32_t)sizeof(struct pt)`: both operands are `uint32_t` (and `uint32_t` is not promoted, since it isn't narrower than `int`), so the
  product is computed modulo 2^32. The smallest wrapping count is `n = ceil(2^32 / 12) = 357913942`:
  `357913942 * 12 = 4294967304 = 2^32 + 8`, so `bytes == 8`.
- 8 is far below `POINTS_MAX_BYTES`, so the limit check, applied to the *wrapped product*, passes. `malloc(8)` succeeds, and the first loop
  iteration already writes 12 bytes into an 8-byte block. ASan: `heap-buffer-overflow`, "0 bytes after 8-byte region" on the
  write of `p[0].z` (offset 8, size 4).
- Other breaking values are `357913942 + k` for small `k`, giving `bytes = 8 + 12k`.
- **Fix:** check the *count* against `POINTS_MAX_BYTES / sizeof(struct pt)` before multiplying, and multiply in `size_t`. Checking a
  product after it has wrapped can't work, because the information is already gone.
- `TAG = PREDICT` (verify-all only requires the starter to build). The exercise is `[BREAK]`, and the starter's own `make check`
  passes, because the break needs your `n`. The solution's harness adds the regression checks.
