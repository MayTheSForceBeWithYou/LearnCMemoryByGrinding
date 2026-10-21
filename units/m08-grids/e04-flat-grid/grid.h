#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
#include <stddef.h>

/* A w x h grid of char cells stored row-major in one block: cell (x, y) is cells[y * w + x].
 * Ownership: a Grid and its cells are owned by whoever called grid_create, and released
 * together by grid_destroy. Nothing else frees them. */
typedef struct {
    size_t w, h;
    char  *cells;     /* w * h bytes, owned by the Grid */
} Grid;

/* Returns a new grid with every cell set to fill, or NULL if w == 0, h == 0,
 * w * h overflows size_t, or allocation fails. Leaks nothing on failure. */
Grid *grid_create(size_t w, size_t h, char fill);

/* Frees the cells and the grid. Accepts NULL. */
void grid_destroy(Grid *g);

/* Out-of-range (x, y): return false and leave *out untouched. */
bool grid_get(const Grid *g, size_t x, size_t y, char *out);
bool grid_set(Grid *g, size_t x, size_t y, char c);

#endif
