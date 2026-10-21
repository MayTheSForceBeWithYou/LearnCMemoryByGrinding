# m08-e05-one-block `[WRITE]` (M)

## Goal

Allocate a `w × h` grid of `long double` that supports `g[y][x]` and is released with a single
`free(g)`: the row-pointer table and the cells live in **one** allocation.

```c
long double **grid2d_alloc(size_t w, size_t h);   /* cells zeroed; free with free() */
```

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- Exactly one call to an allocation function per grid.
- All cells read as `0.0L` after allocation.
- The cell area is aligned for `long double` (`_Alignof(long double)`, 16 on x86-64) **by construction**:
  compute the padding from the alignment and the table size. With 8-byte row pointers and an odd height,
  the table does not end on a 16-byte boundary.
- Return NULL for `w == 0`, `h == 0`, any size computation that would overflow `size_t`, or allocation failure.
- Writing every cell must not touch any byte of the row table. The harness checks this.

## Verify

```bash
make check
```

The harness prints `m08-e05-one-block: all checks passed`. It uses odd heights and checks the
first cell's address modulo the alignment. That check is the grader. UBSan's alignment check, part of
`make asan`, also fires on the first load from a misaligned cell (the harness reads the cells before it writes them).

## Afterwards

In two or three sentences in `NOTES.md`, compare this layout with e04's: what you gained, and what
you can no longer do. In particular, what would it take to resize one dimension, and can rows differ in length?
