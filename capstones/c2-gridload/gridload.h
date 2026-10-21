#ifndef GRIDLOAD_H
#define GRIDLOAD_H

#include <stddef.h>
#include <stdio.h>

/* TODO: write the ownership contract for Grid and for every function below:
 * who owns what before and after each call, on success and on failure,
 * which arguments may be NULL, and how long returned pointers stay valid. */

#define GRID_MAX_DIM 4096

typedef struct Grid Grid;

typedef enum {
    GRID_OK = 0,
    GRID_ERR_IO,        /* could not open or read the file */
    GRID_ERR_EMPTY,     /* no tiles at all */
    GRID_ERR_RAGGED,    /* rows of different width, or a blank row */
    GRID_ERR_TILE,      /* illegal character */
    GRID_ERR_SPAWN,     /* zero or more than one '@' */
    GRID_ERR_TOO_BIG,   /* width or height over GRID_MAX_DIM */
    GRID_ERR_OOM        /* allocation failed */
} GridErr;

GridErr     grid_load(const char *path, Grid **out);
GridErr     grid_load_stream(FILE *f, Grid **out);
void        grid_destroy(Grid *g);
size_t      grid_width(const Grid *g);
size_t      grid_height(const Grid *g);
char        grid_at(const Grid *g, size_t x, size_t y);
void        grid_spawn(const Grid *g, size_t *x, size_t *y);
const char *grid_strerror(GridErr e);

#endif
