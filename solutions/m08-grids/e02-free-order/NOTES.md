# m08-e02-free-order: notes

**The bug.** `board_destroy` frees the `Board` first and then reads `b->h` and `b->rows` from the
freed block to find the rows. The fix is to reverse the order: rows, then the table, then the struct.
Free a container only after the last read of its fields.

**Why the plain run passes.** `free` doesn't erase memory. glibc's tcache writes its own bookkeeping into the
first 16 bytes of a freed chunk, and `h` and `rows` sit at offsets 24 and 32, behind the 16-byte `name`.
So they still hold their old values when they are read back, and nothing visibly breaks.

**How the tools show it.** gcc 16 already warns at compile time (`-Wuse-after-free`: "pointer 'b'
used after 'free'"). ASan reports `heap-use-after-free ... READ of size 8`, at the loop condition
(`main.c:47`), with "freed by" pointing at `free(b)` on line 46 and "previously allocated by" at the
`calloc` in `board_create`. Valgrind reports `Invalid read of size 8 ... 24 bytes inside a block of
size 40 free'd`. The 24 is the offset of `h`.

**The usual mistake.** Fixing it by copying `b->rows` into a local before `free(b)`. That works, but it
keeps the wrong order and the next reader inherits the trap. Reorder instead.
