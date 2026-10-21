# m11-e01-shadow-bytes: notes

**Reading.** The `=>` row starts at `...0000`: `fa fa` is bytes `0x00`-`0x0f` (left redzone),
`00` is `0x10`-`0x17` (8 addressable bytes), `[06]` is `0x18`-`0x1f`, of which the first 6
are addressable. The block is 8 + 6 = 14 bytes, which matches `malloc(n * 2)` with `n = 7`.
The fault address `...001e` is byte 14 of the block (`0x1e - 0x10`), the first byte after it,
inside the bracketed granule. The write is `out[2 * n] = '\0'` (line 21), the terminator.

**Fix.** `malloc(n * 2 + 1)`. The loop writes exactly `2n` digits; only the NUL had no room.
The guard `n > (SIZE_MAX - 1) / 2` already ensures `2n + 1` does not wrap.

**Common mistake.** Changing the loop bound or writing the NUL at `2 * n - 1`, which silently
truncates the last digit. The check compares the full string and catches it.

**Why the plain build passes.** glibc rounds `malloc(14)` up to a chunk with 24 usable
bytes, so byte 14 lands in slack that belongs to the same chunk and nothing breaks.
ASan poisons at byte granularity (the `06` in the last granule), and Valgrind's allocator
puts a redzone directly after byte 13. Both catch it.
