#ifndef GRIDLOAD_H
#define GRIDLOAD_H

#include <stddef.h>
#include <stdio.h>

/* Ownership contract
 * ------------------
 * A Grid is opaque. It is created only by grid_load / grid_load_stream, which hand it to the
 * caller through *out; the caller owns it and releases it with exactly one grid_destroy.
 * A Grid owns its cells (one contiguous block). Nothing returned by these functions points
 * into a Grid, so no accessor result outlives anything.
 * Loading is all-or-nothing: on any error *out is NULL and nothing allocated by the call is
 * still allocated.
 * Unless stated otherwise, pointer arguments must not be NULL. */

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

/* Opens path, loads it, closes it on every path. path is borrowed.
 * GRID_OK: *out is a new Grid owned by the caller. Otherwise: *out == NULL. */
GridErr     grid_load(const char *path, Grid **out);

/* Loads from an open stream, reading to end of file or to the first error. f is borrowed:
 * it is not closed, and its position afterwards is unspecified. Same *out rules as grid_load. */
GridErr     grid_load_stream(FILE *f, Grid **out);

/* Frees the grid and its cells. Accepts NULL. */
void        grid_destroy(Grid *g);

/* Accessors. g must be a live Grid (not NULL). */
size_t      grid_width(const Grid *g);
size_t      grid_height(const Grid *g);
/* The tile at (x, y), or '\0' if x >= width or y >= height. */
char        grid_at(const Grid *g, size_t x, size_t y);
/* Stores the spawn coordinates through x and y (neither may be NULL). */
void        grid_spawn(const Grid *g, size_t *x, size_t *y);

/* A static, never-NULL description of e; the caller must not free it. Unknown values
 * give "unknown error". */
const char *grid_strerror(GridErr e);

#endif
