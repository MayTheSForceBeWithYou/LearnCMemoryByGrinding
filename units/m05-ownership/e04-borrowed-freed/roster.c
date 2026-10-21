#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "roster.h"

struct roster {
    char **names;
    size_t len, cap;
};

struct roster *roster_create(void)
{
    return calloc(1, sizeof(struct roster));
}

int roster_add(struct roster *r, const char *name)
{
    if (r->len == r->cap) {
        size_t nc = r->cap ? r->cap * 2 : 4;
        char **tmp = realloc(r->names, nc * sizeof *tmp);
        if (tmp == NULL) {
            return -1;
        }
        r->names = tmp;
        r->cap = nc;
    }
    r->names[r->len++] = (char *)name;
    return 0;
}

size_t roster_size(const struct roster *r)
{
    return r->len;
}

const char *roster_name(const struct roster *r, size_t i)
{
    return r->names[i];
}

void roster_destroy(struct roster *r)
{
    if (r == NULL)
        return;
    for (size_t i = 0; i < r->len; i++)
        free(r->names[i]);
    free(r->names);
    free(r);
}
