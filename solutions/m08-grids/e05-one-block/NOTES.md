# m08-e05-one-block: notes

**Layout.** `[ h row pointers | padding | w*h long doubles ]`, one `calloc`. The padding rounds the table
size up to `_Alignof(long double)`, which is 16 on x86-64. With 8-byte pointers the padding is 8 bytes
whenever `h` is odd and 0 when `h` is even. That's why the harness uses heights 1, 3 and 13: without the
rounding, the first cell lands at an address that is 8 mod 16. The modulo check fails, and UBSan's
alignment check fires on the first `long double` load (the zero check reads before anything is written).

**Overflow.** Four checks guard four operations: `h * sizeof(pointer)`, the round-up `table + a - 1`,
`w * h`, and `off + cells * sizeof(long double)`. The harness's wrap-to-small cases (`(SIZE_MAX / 2 + 2) x 2`
and `(SIZE_MAX / 16 + 2) x 1`) make a missing check return a tiny block, which the `== NULL` checks catch.

**Zeroing.** `calloc` zeroes the cells. All-bits-zero is `0.0L` on IEEE platforms, which C does not
promise in general but every target of this course satisfies. The table is overwritten anyway.

**Compared with e04.** You get `g[y][x]` and one `free`. You lose the ability to resize one dimension
without rebuilding the whole block and every row pointer, and you can't have jagged rows. You also pay
`h` pointers of overhead.

**The usual mistake.** Computing the padding from `sizeof(long double)` instead of `_Alignof`. It happens to
give 16 on x86-64 too, but it's the wrong question. Second: computing the cell start with pointer arithmetic
on `long double **` (`(long double *)(g + h)`), which skips the padding entirely.
