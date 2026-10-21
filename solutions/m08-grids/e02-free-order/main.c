/* m08-e02-free-order: a jagged grid of labelled rows. Builds, runs, and is wrong. */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"

typedef struct {
    char    name[16];   /* label printed above the grid */
    size_t  w, h;
    char  **rows;       /* h rows, each a heap string of w chars + NUL; owned */
} Board;

static void board_destroy(Board *b);

static Board *board_create(const char *name, size_t w, size_t h, char fill)
{
    Board *b = calloc(1, sizeof *b);
    if (!b)
        return NULL;
    snprintf(b->name, sizeof b->name, "%s", name);
    b->w = w;
    b->h = h;
    b->rows = calloc(h, sizeof *b->rows);
    if (!b->rows) {
        free(b);
        return NULL;
    }
    for (size_t y = 0; y < h; y++) {
        b->rows[y] = malloc(w + 1);
        if (!b->rows[y]) {
            board_destroy(b);
            return NULL;
        }
        memset(b->rows[y], fill, w);
        b->rows[y][w] = '\0';
    }
    return b;
}

static void board_destroy(Board *b)
{
    if (!b)
        return;
    for (size_t y = 0; y < b->h; y++)
        free(b->rows[y]);
    free(b->rows);
    free(b);
}

int main(void)
{
    Board *b = board_create("cellar", 6, 3, '.');
    if (!b) {
        fprintf(stderr, "m08-e02-free-order: out of memory\n");
        return EXIT_OOM;
    }
    b->rows[1][2] = '@';
    printf("%s\n", b->name);
    for (size_t y = 0; y < b->h; y++)
        printf("  %s\n", b->rows[y]);
    check(strcmp(b->rows[1], "..@...") == 0, "spawn placed in row 1");
    check(strlen(b->rows[2]) == 6, "rows are 6 wide");
    board_destroy(b);
    return checks_done("m08-e02-free-order");
}
