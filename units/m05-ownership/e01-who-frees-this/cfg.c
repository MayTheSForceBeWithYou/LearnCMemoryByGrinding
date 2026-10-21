#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "cfg.h"

/* A toy config: two fixed keys whose values are rebuilt on reload. */
struct cfg {
    char *path;
    char *name;
    char *mode;
    int generation;
};

static int fill(struct cfg *c)
{
    char *name = strdup(c->generation == 0 ? "hero" : "hero-v2");
    char *mode = strdup("fast");
    if (name == NULL || mode == NULL) {
        free(name);
        free(mode);
        return -1;
    }
    free(c->name);
    free(c->mode);
    c->name = name;
    c->mode = mode;
    return 0;
}

struct cfg *cfg_new(const char *path)
{
    struct cfg *c = calloc(1, sizeof *c);
    if (c == NULL)
        return NULL;
    c->path = strdup(path);
    if (c->path == NULL || fill(c) != 0) {
        cfg_free(c);
        return NULL;
    }
    return c;
}

void cfg_free(struct cfg *c)
{
    if (c == NULL)
        return;
    free(c->path);
    free(c->name);
    free(c->mode);
    free(c);
}

const char *cfg_get(const struct cfg *c, const char *key)
{
    if (strcmp(key, "name") == 0)
        return c->name;
    if (strcmp(key, "mode") == 0)
        return c->mode;
    return NULL;
}

char *cfg_dup(const struct cfg *c, const char *key)
{
    const char *v = cfg_get(c, key);
    return v ? strdup(v) : NULL;
}

const char *cfg_path(const struct cfg *c)
{
    return c->path;
}

int cfg_reload(struct cfg *c)
{
    c->generation++;
    return fill(c);
}
