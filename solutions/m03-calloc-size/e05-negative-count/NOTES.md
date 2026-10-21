# M03 e05 notes

- `neg`'s header is `0xffffffff`, which is `count == -1`. `count > RECORDS_MAX` is false for every negative number, so the check
  "looks" like a bound but is only an upper bound.
- `count + 1` is computed in `int32_t`: `0`. `(size_t)0 * 8 = 0`, and `malloc(0)` succeeds with a minimum-size block (0 usable bytes as
  far as ASan is concerned).
- The loop condition `i != count` is never false for `count == -1` (i counts up from 0), so the loop runs until the input runs out,
  copying 3 records (24 bytes) into a 0-byte block: `heap-buffer-overflow`, "0 bytes after 1-byte region" (ASan reports
  `malloc(0)` as a 1-byte region).
- **Fix:** `!(count >= 0 && count <= RECORDS_MAX)`, done while `count` is still signed. After that, `count + 1` can't overflow,
  and converting to `size_t` is value-preserving.
- Do not write `0 <= count <= RECORDS_MAX`. It parses as `(0 <= count) <= RECORDS_MAX`, which is always true.
- The short-input path frees the block before returning NULL. That was already right, and the fix keeps it.
