# M04 e04 notes

Observed on the course machine (Arch glibc, gcc 16):

| # | plain | valgrind | asan |
|---|---|---|---|
| 1 | clean | clean | clean |
| 2 | runs, prints "tail byte != 'a'" (the value is indeterminate) | `Conditional jump or move depends on uninitialised value(s)` | silent: ASan doesn't track initialisation |
| 3 | prints `b` for both offsets: the stale byte is still there | `Invalid read of size 1` (offset 16) | `heap-buffer-overflow` (offset 16) |
| 4 | `realloc(p, 0)` returned NULL (glibc freed `p`), then `free(): double free detected in tcache 2`, exit 134 | `Invalid free()` | `attempting double-free` |
| 5 | 114 moves out of 10000 grows | | 10000 moves |
| 6 | 4 moves out of 14 grows | | 14 moves |

- **2:** realloc's grown tail is never initialised, even when its first 32 bytes came from a `memset`.
- **3:** after a shrink, offset 16 is outside the block. The plain build reads the old byte, because glibc split the chunk in place.
- **4:** this is the course's reason for the ban. On glibc, `realloc(p, 0)` frees `p` and returns NULL. On other libcs it returns a new
  block and leaves `p` freed (or not). With the `tmp` pattern, `NULL` looks like failure, so the "keep the old block" path then frees or uses
  a freed block, as here. C23 makes the call UB outright.
- **5 vs 6:** with an unrelated allocation after each grow, glibc can still grow in place while the next chunk is free (small grows stay inside
  one 16-byte-granular chunk), so plain moves are rare (114). But each move copies the whole buffer, and the number of grows (10000) is
  what makes `+1` quadratic in the worst case. Doubling needs 14 grows in total. ASan moves on *every* realloc, which shows the worst
  case directly: 10000 copies vs 14.
