/* m08-e05-one-block: row table and cells in one allocation, g[y][x] access. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "course.h"

/* Returns a w x h grid of zeroed long double cells, usable as g[y][x], made by exactly
 * one allocation and released by free(g). The cell area is aligned for long double.
 * NULL if w == 0, h == 0, any size computation overflows, or allocation fails. */
long double **grid2d_alloc(size_t w, size_t h);

static int exercise(size_t w, size_t h)
{
    long double **g = grid2d_alloc(w, h);
    if (!g)
        return 0;
    long double *first = &g[0][0];
    check((uintptr_t)first % _Alignof(long double) == 0, "cell area aligned for long double");
    int zero = 1;
    for (size_t y = 0; y < h; y++)
        for (size_t x = 0; x < w; x++)
            zero &= g[y][x] == 0.0L;
    check(zero, "cells start at zero");
    for (size_t y = 0; y < h; y++)
        for (size_t x = 0; x < w; x++)
            g[y][x] = (long double)(y * 1000 + x) + 0.5L;
    int table_ok = 1, cells_ok = 1;
    for (size_t y = 0; y < h; y++) {
        table_ok &= g[y] == first + y * w;
        for (size_t x = 0; x < w; x++)
            cells_ok &= g[y][x] == (long double)(y * 1000 + x) + 0.5L;
    }
    check(table_ok, "writing cells did not touch the row table");
    check(cells_ok, "every cell holds what was written");
    check((char *)first > (char *)g && (char *)first >= (char *)(g + h), "cells come after the table");
    free(g);
    return 1;
}

int main(void)
{
    check(grid2d_alloc(0, 4) == NULL, "w == 0 rejected");
    check(grid2d_alloc(4, 0) == NULL, "h == 0 rejected");
    check(grid2d_alloc(SIZE_MAX / 4, 8) == NULL, "w * h overflow rejected");
    check(grid2d_alloc(2, SIZE_MAX / 4) == NULL, "table size overflow rejected");
    /* Sizes that wrap around to a tiny block: without the checks, calloc succeeds. */
    long double **wrapped = grid2d_alloc(SIZE_MAX / 2 + 2, 2);        /* w * h wraps to 2 */
    check(wrapped == NULL, "w * h wrapping to a small size rejected");
    free(wrapped);
    wrapped = grid2d_alloc(SIZE_MAX / 16 + 2, 1);                     /* cell bytes wrap to 16 */
    check(wrapped == NULL, "cell byte count wrapping to a small size rejected");
    free(wrapped);
    /* Odd heights: 8-byte row pointers do not end on a 16-byte boundary. */
    int ok = exercise(1, 1) && exercise(5, 3) && exercise(4, 13) && exercise(3, 2);
    if (!ok) {
        fprintf(stderr, "m08-e05-one-block: out of memory\n");
        return EXIT_OOM;
    }
    return checks_done("m08-e05-one-block");
}

/* ---- Your definition of grid2d_alloc goes below this line. ---- */

long double **grid2d_alloc(size_t w, size_t h)
{
    if (w == 0 || h == 0)
        return NULL;
    if (h > SIZE_MAX / sizeof(long double *) || w > SIZE_MAX / h)
        return NULL;
    size_t table = h * sizeof(long double *);
    size_t a = _Alignof(long double);
    if (table > SIZE_MAX - (a - 1))
        return NULL;
    size_t off = (table + a - 1) / a * a;          /* round up to the cell alignment */
    size_t cells = w * h;
    if (cells > (SIZE_MAX - off) / sizeof(long double))
        return NULL;

    char *block = calloc(1, off + cells * sizeof(long double));
    if (!block)
        return NULL;
    long double **rows = (long double **)(void *)block;
    long double *data = (long double *)(void *)(block + off);
    for (size_t y = 0; y < h; y++)
        rows[y] = data + y * w;
    return rows;
}
