/* gridload.c: your implementation of gridload.h goes here.
 * Read with fgetc/fgets/fread into buffers you allocate yourself. Do not use getline,
 * getdelim or asprintf: the fault-injection build cannot see their allocations. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gridload.h"

struct Grid {
    size_t w, h;
    size_t spawn_x, spawn_y;
    char  *cells;               /* w * h tiles, row-major, owned */
};

static int legal_tile(int c)
{
    return c == '#' || c == '.' || c == '~' || c == '+' || c == '@';
}

/* Appends c to the growing cell buffer. 0 on OOM (buffer untouched). */
static int put_cell(char **cells, size_t *len, size_t *cap, char c)
{
    if (*len == *cap) {
        size_t new_cap = *cap ? *cap * 2 : 256;     /* bounded: len <= MAX*MAX + MAX */
        char *tmp = realloc(*cells, new_cap);
        if (!tmp)
            return 0;
        *cells = tmp;
        *cap = new_cap;
    }
    (*cells)[(*len)++] = c;
    return 1;
}

GridErr grid_load_stream(FILE *f, Grid **out)
{
    *out = NULL;
    char *cells = NULL;
    size_t len = 0, cap = 0;
    size_t w = 0, h = 0, x = 0, blank = 0, spawns = 0, sx = 0, sy = 0;
    GridErr err = GRID_OK;

    for (;;) {
        int c = fgetc(f);
        int eol = 0, eof = 0;
        if (c == EOF) {
            eof = 1;
        } else if (c == '\n') {
            eol = 1;
        } else if (c == '\r') {
            int next = fgetc(f);
            if (next == '\n') {
                eol = 1;
            } else if (next == EOF && ferror(f)) {
                err = GRID_ERR_IO;
                goto fail;
            } else {
                err = GRID_ERR_TILE;                /* a CR not ending a line, even at EOF */
                goto fail;
            }
        }
        if (eof && ferror(f)) {
            err = GRID_ERR_IO;
            goto fail;
        }

        if (eol || (eof && x > 0)) {                /* end of a row */
            if (x == 0) {
                blank++;
            } else {
                if (h == 0)
                    w = x;
                else if (x != w) {
                    err = GRID_ERR_RAGGED;
                    goto fail;
                }
                if (++h > GRID_MAX_DIM) {
                    err = GRID_ERR_TOO_BIG;
                    goto fail;
                }
            }
            x = 0;
        } else if (!eof) {                          /* a tile */
            if (!legal_tile(c)) {
                err = GRID_ERR_TILE;
                goto fail;
            }
            if (c == '@') {
                if (++spawns > 1) {
                    err = GRID_ERR_SPAWN;
                    goto fail;
                }
                sx = x;
                sy = h;
            }
            if (++x > GRID_MAX_DIM) {
                err = GRID_ERR_TOO_BIG;
                goto fail;
            }
            if (h > 0 && x > w) {
                err = GRID_ERR_RAGGED;
                goto fail;
            }
            if (!put_cell(&cells, &len, &cap, (char)c)) {
                err = GRID_ERR_OOM;
                goto fail;
            }
        }
        if (eof)
            break;
    }

    if (h == 0)
        err = GRID_ERR_EMPTY;
    else if (blank > 0)
        err = GRID_ERR_RAGGED;
    else if (spawns == 0)
        err = GRID_ERR_SPAWN;
    if (err != GRID_OK)
        goto fail;

    Grid *g = malloc(sizeof *g);
    if (!g) {
        err = GRID_ERR_OOM;
        goto fail;
    }
    g->w = w;
    g->h = h;
    g->spawn_x = sx;
    g->spawn_y = sy;
    g->cells = cells;                               /* len == w * h here */
    *out = g;
    return GRID_OK;

fail:
    free(cells);
    return err;
}

GridErr grid_load(const char *path, Grid **out)
{
    *out = NULL;
    FILE *f = fopen(path, "rb");
    if (!f)
        return GRID_ERR_IO;
    GridErr err = grid_load_stream(f, out);
    if (fclose(f) != 0 && err == GRID_OK) {
        grid_destroy(*out);
        *out = NULL;
        err = GRID_ERR_IO;
    }
    return err;
}

void grid_destroy(Grid *g)
{
    if (!g)
        return;
    free(g->cells);
    free(g);
}

size_t grid_width(const Grid *g) { return g->w; }
size_t grid_height(const Grid *g) { return g->h; }

char grid_at(const Grid *g, size_t x, size_t y)
{
    if (x >= g->w || y >= g->h)
        return '\0';
    return g->cells[y * g->w + x];
}

void grid_spawn(const Grid *g, size_t *x, size_t *y)
{
    *x = g->spawn_x;
    *y = g->spawn_y;
}

const char *grid_strerror(GridErr e)
{
    switch (e) {
    case GRID_OK:          return "ok";
    case GRID_ERR_IO:      return "cannot open or read the file";
    case GRID_ERR_EMPTY:   return "map has no tiles";
    case GRID_ERR_RAGGED:  return "rows differ in width (or a row is blank)";
    case GRID_ERR_TILE:    return "illegal character in map";
    case GRID_ERR_SPAWN:   return "map needs exactly one '@'";
    case GRID_ERR_TOO_BIG: return "map wider or taller than GRID_MAX_DIM";
    case GRID_ERR_OOM:     return "out of memory";
    }
    return "unknown error";
}
