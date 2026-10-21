# m12-e06-off-by-one-nul: notes

**Bug.** `malloc(n)` then `out[n] = '\0'`: the terminator is written one byte past the block.

**Mechanism.** ASan: `heap-buffer-overflow`, a 1-byte WRITE `0 bytes after 8-byte region` in
`prefix`. Memcheck: `Invalid write of size 1`, because its allocator puts a redzone directly
after the requested 8 bytes. Plain glibc: `malloc(8)` has 24 usable bytes, so byte 8 is the
chunk's own slack. Nothing can ever observe it at this size.

**Fix.** `malloc(n + 1)`.

**Why this is its own class.** It is the most common overflow in C string code, and the
least visible: exactly one byte, exactly at `size`, nearly always hidden by slack.
