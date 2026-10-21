# m12-e11-mixed-a: notes

**Bug 1, growth from zero.** `ncap = b->cap * 2` with `cap == 0` stays 0, so
`realloc(NULL, 0)` returns a minimum-size block, and the first entry is written past its
end. ASan: `heap-buffer-overflow`, `WRITE of size 8` in `book_add` (line 36),
`0 bytes after 1-byte region`, because ASan backs a 0-byte request with 1 byte. Memcheck:
`0 bytes after a block of size 0 alloc'd` by `realloc`, then Valgrind aborts on its own
corrupted metadata (`bszB_lo == bszB_hi`). Plain build: `double free or corruption (out)`.
(`realloc(p, 0)` is implementation-defined in C17 and undefined in C23: M04.) Fix: `ncap = b->cap ? b->cap * 2 : 4`.

**Bug 2, partial free.** `book_remove` frees `name` but not `number`, leaking one 9-byte
block per removal. Only visible once bug 1 is fixed, because ASan halts at the first report.
LSan: `Direct leak of 9 byte(s)` allocated by `strdup` in `book_add`. Fix: free `number` too.

Bug 1 masks bug 2 because the overflow aborts the program before exit, so LSan never runs.
