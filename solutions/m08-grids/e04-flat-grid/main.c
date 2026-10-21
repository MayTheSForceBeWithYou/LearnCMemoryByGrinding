/* m08-e04-flat-grid: a row-major w*h grid in one block. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"
#include "grid.h"

int main(void)
{
    check(grid_create(0, 5, '.') == NULL, "w == 0 rejected");
    check(grid_create(5, 0, '.') == NULL, "h == 0 rejected");
    check(grid_create(SIZE_MAX / 2, 3, '.') == NULL, "w * h overflow rejected");
    check(grid_create(3, SIZE_MAX / 2, '.') == NULL, "h * w overflow rejected");
    /* Products that wrap around to a tiny size: without an overflow test, malloc(2) succeeds. */
    Grid *wrapped = grid_create(SIZE_MAX / 2 + 2, 2, '.');
    check(wrapped == NULL, "w * h wrapping to 2 bytes rejected");
    grid_destroy(wrapped);
    wrapped = grid_create(2, SIZE_MAX / 2 + 2, '.');
    check(wrapped == NULL, "h * w wrapping to 2 bytes rejected");
    grid_destroy(wrapped);

    Grid *g = grid_create(7, 3, '.');
    if (!g) {
        fprintf(stderr, "m08-e04-flat-grid: out of memory\n");
        return EXIT_OOM;
    }
    check(g->w == 7 && g->h == 3, "dimensions stored");
    int filled = 1;
    for (size_t i = 0; i < 21; i++)
        filled &= g->cells[i] == '.';
    check(filled, "every cell filled");

    check(grid_set(g, 6, 0, 'a'), "set top-right");
    check(grid_set(g, 0, 2, 'b'), "set bottom-left");
    check(grid_set(g, 6, 2, 'c'), "set bottom-right");
    check(g->cells[6] == 'a' && g->cells[14] == 'b' && g->cells[20] == 'c', "row-major layout");

    char c = 'z';
    check(grid_get(g, 0, 2, &c) && c == 'b', "get");
    c = 'z';
    check(!grid_get(g, 7, 0, &c) && c == 'z', "x == w out of range, out untouched");
    check(!grid_get(g, 0, 3, &c) && c == 'z', "y == h out of range");
    check(!grid_set(g, SIZE_MAX, 1, '!'), "huge x rejected");
    check(!grid_set(g, 1, SIZE_MAX, '!'), "huge y rejected");
    check(memchr(g->cells, '!', 21) == NULL, "rejected sets wrote nothing");

    grid_destroy(g);
    grid_destroy(NULL);
    return checks_done("m08-e04-flat-grid");
}

/* ---- Your definitions of the grid.h functions go below this line. ---- */

Grid *grid_create(size_t w, size_t h, char fill)
{
    if (w == 0 || h == 0 || w > SIZE_MAX / h)
        return NULL;
    Grid *g = malloc(sizeof *g);
    if (!g)
        return NULL;
    g->cells = malloc(w * h);
    if (!g->cells) {
        free(g);
        return NULL;
    }
    memset(g->cells, fill, w * h);
    g->w = w;
    g->h = h;
    return g;
}

void grid_destroy(Grid *g)
{
    if (!g)
        return;
    free(g->cells);
    free(g);
}

/* The one place that knows the layout. Returns NULL when (x, y) is outside the grid. */
static char *cell(const Grid *g, size_t x, size_t y)
{
    if (x >= g->w || y >= g->h)
        return NULL;
    return &g->cells[y * g->w + x];
}

bool grid_get(const Grid *g, size_t x, size_t y, char *out)
{
    const char *p = cell(g, x, y);
    if (!p)
        return false;
    *out = *p;
    return true;
}

bool grid_set(Grid *g, size_t x, size_t y, char c)
{
    char *p = cell(g, x, y);
    if (!p)
        return false;
    *p = c;
    return true;
}
