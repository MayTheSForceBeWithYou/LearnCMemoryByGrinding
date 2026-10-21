# M07 e03 notes

- Two passes: measure (with an overflow check on every addition), then copy at a moving write pointer. One allocation, sized
  `sum(len) + (n - 1) * len(sep) + 1`.
- `strlen` runs twice per part, which is still linear. Caching the lengths would need a second allocation, and that's not worth it here.
- `strcat` in a loop would rescan the growing result every time, which is O(total^2): 10000 parts means about 2.5e8 byte reads, and
  under ASan that's slow.
- `n == 0`: `total == 1`, so `malloc(1)` holds `""`. Returning NULL would be ambiguous with failure. Returning a literal `""` would make
  `free` invalid.
- `(i > 0 ? lsep : 0)` places separators *between* parts. A trailing separator is the usual off-by-one.
