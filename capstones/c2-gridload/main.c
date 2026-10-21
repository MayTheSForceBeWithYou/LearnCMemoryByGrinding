/* c2-gridload: harness (no arguments) and CLI (`prog MAPFILE`).
 * CLI exit codes: 0 loaded, 2 bad input or usage, 3 out of memory. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"
#include "gridload.h"

typedef struct {
    const char *file;
    GridErr     want;
    size_t      w, h, sx, sy;      /* checked when want == GRID_OK */
} Case;

static const Case cases[] = {
    {"maps/good-small.txt",         GRID_OK, 5, 3, 2, 1},
    {"maps/good-crlf.txt",          GRID_OK, 5, 3, 2, 1},
    {"maps/good-noeol.txt",         GRID_OK, 3, 3, 1, 1},
    {"maps/good-wide.txt",          GRID_OK, 300, 40, 150, 20},
    {"maps/good-maxwidth.txt",      GRID_OK, 4096, 1, 0, 0},
    {"maps/good-maxheight.txt",     GRID_OK, 1, 4096, 0, 0},
    {"maps/bad-toowide.txt",        GRID_ERR_TOO_BIG, 0, 0, 0, 0},
    {"maps/bad-tootall.txt",        GRID_ERR_TOO_BIG, 0, 0, 0, 0},
    {"maps/bad-ragged.txt",         GRID_ERR_RAGGED, 0, 0, 0, 0},
    {"maps/bad-trailing-blank.txt", GRID_ERR_RAGGED, 0, 0, 0, 0},
    {"maps/bad-mid-blank.txt",      GRID_ERR_RAGGED, 0, 0, 0, 0},
    {"maps/bad-tile.txt",           GRID_ERR_TILE, 0, 0, 0, 0},
    {"maps/bad-lone-cr.txt",        GRID_ERR_TILE, 0, 0, 0, 0},
    {"maps/bad-cr-eof.txt",         GRID_ERR_TILE, 0, 0, 0, 0},
    {"maps/bad-nospawn.txt",        GRID_ERR_SPAWN, 0, 0, 0, 0},
    {"maps/bad-twospawn.txt",       GRID_ERR_SPAWN, 0, 0, 0, 0},
    {"maps/bad-empty.txt",          GRID_ERR_EMPTY, 0, 0, 0, 0},
    {"maps/bad-crlf-only.txt",      GRID_ERR_EMPTY, 0, 0, 0, 0},
    {"maps/does-not-exist.txt",     GRID_ERR_IO, 0, 0, 0, 0},
};

static int run_harness(void)
{
    static char not_null;
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        const Case *c = &cases[i];
        Grid *g = (Grid *)(void *)&not_null;      /* must be overwritten on every path */
        GridErr e = grid_load(c->file, &g);
        if (e == GRID_ERR_OOM) {
            check(g == NULL, "OOM leaves *out NULL");
            fprintf(stderr, "c2-gridload: out of memory loading %s\n", c->file);
            return EXIT_OOM;
        }
        if (e != c->want)
            printf("%s: got \"%s\", want \"%s\"\n", c->file, grid_strerror(e), grid_strerror(c->want));
        check(e == c->want, "error code");
        if (e != GRID_OK) {
            check(g == NULL, "*out is NULL on error");
            continue;
        }
        check(g != NULL, "*out set on success");
        size_t sx = 99, sy = 99;
        grid_spawn(g, &sx, &sy);
        check(grid_width(g) == c->w && grid_height(g) == c->h, "dimensions");
        check(sx == c->sx && sy == c->sy, "spawn position");
        check(grid_at(g, sx, sy) == '@', "spawn tile is '@'");
        check(grid_at(g, c->w, 0) == '\0' && grid_at(g, 0, c->h) == '\0', "out of range gives '\\0'");
        check(grid_at(g, c->w - 1, c->h - 1) != '\0', "last cell readable");
        grid_destroy(g);
    }
    check(strcmp(grid_strerror(GRID_ERR_TILE), grid_strerror(GRID_ERR_RAGGED)) != 0, "distinct messages");
    check(grid_strerror((GridErr)42) != NULL, "unknown code still gives a string");
    grid_destroy(NULL);

    Grid *g = NULL;                               /* the stream entry point, and tile placement */
    FILE *f = fopen("maps/good-small.txt", "rb");
    if (!f) {
        check(0, "open maps/good-small.txt");
        return checks_done("c2-gridload");
    }
    GridErr e = grid_load_stream(f, &g);
    fclose(f);
    if (e == GRID_ERR_OOM) {
        fprintf(stderr, "c2-gridload: out of memory\n");
        return EXIT_OOM;
    }
    check(e == GRID_OK, "grid_load_stream loads an open file");
    check(g && grid_at(g, 0, 0) == '#' && grid_at(g, 1, 1) == '.' && grid_at(g, 4, 2) == '#', "tiles");
    grid_destroy(g);
    return checks_done("c2-gridload");
}

int main(int argc, char **argv)
{
    if (argc == 1)
        return run_harness();
    if (argc != 2) {
        fprintf(stderr, "usage: %s [MAPFILE]\n", argv[0]);
        return 2;
    }
    Grid *g = NULL;
    GridErr e = grid_load(argv[1], &g);
    if (e != GRID_OK) {
        fprintf(stderr, "gridload: %s: %s\n", argv[1], grid_strerror(e));
        return e == GRID_ERR_OOM ? EXIT_OOM : 2;
    }
    size_t sx, sy;
    grid_spawn(g, &sx, &sy);
    printf("%zu x %zu, spawn at (%zu, %zu)\n", grid_width(g), grid_height(g), sx, sy);
    grid_destroy(g);
    return 0;
}
