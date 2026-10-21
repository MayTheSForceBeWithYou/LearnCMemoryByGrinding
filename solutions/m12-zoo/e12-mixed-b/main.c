/* m12-e12-mixed-b [BUG]: three bugs. */
#include <stdlib.h>
#include "course.h"

typedef struct {
    int **rows; /* owned table of h owned rows of w ints */
    size_t w, h;
} Grid;

static void grid_free(Grid *g)
{
    if (!g->rows)
        return;
    for (size_t y = 0; y < g->h; y++)
        free(g->rows[y]);
    free(g->rows);
    g->rows = NULL;
}

/* Makes g a w x h grid of zeros. Returns 0, or -1 on allocation failure
 * with nothing allocated. */
static int grid_init(Grid *g, size_t w, size_t h)
{
    g->w = w;
    g->h = h;
    g->rows = calloc(h, sizeof *g->rows);
    if (!g->rows)
        return -1;
    for (size_t y = 0; y < h; y++) {
        g->rows[y] = calloc(w, sizeof *g->rows[y]);
        if (!g->rows[y]) {
            grid_free(g);
            return -1;
        }
    }
    return 0;
}

static long grid_sum(const Grid *g)
{
    long sum = 0;
    for (size_t y = 0; y < g->h; y++)
        for (size_t x = 0; x < g->w; x++)
            sum += g->rows[y][x];
    return sum;
}

int main(void)
{
    Grid g;
    if (grid_init(&g, 5, 3) != 0) {
        fprintf(stderr, "m12-e12: out of memory\n");
        return EXIT_OOM;
    }
    check(grid_sum(&g) == 0, "a new grid is all zeros");
    g.rows[1][4] = 7;
    g.rows[2][0] = 5;
    check(grid_sum(&g) == 12, "sum sees both writes");
    grid_free(&g);
    return checks_done("m12-e12-mixed-b");
}
