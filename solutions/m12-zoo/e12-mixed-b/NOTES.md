# m12-e12-mixed-b: notes

**Bug 1, under-allocation.** Rows are `malloc(w)`: `w` bytes, not `w` ints. ASan:
`READ of size 4` in `grid_sum` (line 44), `0 bytes after 5-byte region`. The read of
element 1 covers bytes 4-7, and the shadow byte `05` marks only bytes 0-4 valid. Memcheck
words the same read as `3 bytes after a block of size 5`, counting from the start of the
4-byte access.

**Bug 2, use after free.** `grid_free` frees the table first and then reads `g->rows[y]`
from the freed table. ASan: `heap-use-after-free` in `grid_free`, freed by the `free(g->rows)`
just above. Plain glibc: `free(): invalid pointer`. The freed table's first words now hold
tcache metadata, so `free` is handed a pointer that is not one it returned. Fix: free the rows, then the table.

**Bug 3, uninitialised cells.** `malloc` does not zero, but the contract says "grid of zeros".
Only memcheck reports it (conditional jump in `main`'s first check, origin `grid_init`'s
`malloc`). ASan's `0xbe` fill makes the first check fail without a report. Plain glibc happens
to return zeroed top-chunk memory. Bugs 1 and 3 share a line, and one call fixes both:
`calloc(w, sizeof *g->rows[y])`.
