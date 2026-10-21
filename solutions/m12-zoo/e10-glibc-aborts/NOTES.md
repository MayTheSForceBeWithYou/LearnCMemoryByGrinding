# m12-e10-glibc-aborts: notes

**Part A.** `reader_close` frees but does not reset `r->buf`, so the documented second close
frees the same chunk again. glibc: `free(): double free detected in tcache 2`, raised inside the
second `free`. The 12-byte chunk went to the tcache, and its key word matched on the second free.
Fix: `r->buf = NULL;` after `free` (`free(NULL)` is a no-op), which makes the contract true.

**Part B.** `pad_right` allocates `width + 1` bytes but copies all of `s` when `s` is longer.
The 61-byte copy into a 41-byte request (40 usable bytes) runs into the next chunk header.
That chunk is the top chunk: the block was freshly carved from top, after the tcache's own
bookkeeping block that glibc allocated at the first `free`. Bytes 40-47 of the label
(`"n-and-th"`) become top's size field. The bug is in the first `pad_right` call, but the
abort comes at the *next* allocation served from top: the 2001-byte `malloc` in the second
call. glibc checks top's size against the memory the arena actually has:
`malloc(): corrupted top size`.
Fix: allocate `(width > len ? width : len) + 1`.

*Why 40 columns.* With an 8-column cell, the 9-byte request reuses the chunk freed in
Part A, which borders the tcache block and not top. The same overflow then goes completely
unnoticed by glibc (tried while building this exercise). Neighbourhood decides what glibc can see.

Valgrind reports both as `Invalid free` and `Invalid write` at the actual faulty lines,
because its allocator detects them before glibc's structures are involved.
