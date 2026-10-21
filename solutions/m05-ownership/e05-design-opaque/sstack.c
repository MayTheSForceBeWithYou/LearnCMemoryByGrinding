#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sstack.h"

struct sstack {
    char **items;      /* each owned */
    size_t len, cap;
};

sstack *sstack_create(void)
{
    return calloc(1, sizeof(sstack));
}

void sstack_destroy(sstack *s)
{
    if (s == NULL)
        return;
    for (size_t i = 0; i < s->len; i++)
        free(s->items[i]);
    free(s->items);
    free(s);
}

int sstack_push(sstack *s, const char *str)
{
    if (s->len == s->cap) {
        size_t nc = s->cap ? s->cap * 2 : 8;
        if (nc > SIZE_MAX / sizeof *s->items)
            return -1;
        char **tmp = realloc(s->items, nc * sizeof *tmp);
        if (tmp == NULL)
            return -1;
        s->items = tmp;           /* growing alone leaves the stack's contents unchanged */
        s->cap = nc;
    }
    char *copy = strdup(str);
    if (copy == NULL)
        return -1;
    s->items[s->len++] = copy;
    return 0;
}

char *sstack_pop(sstack *s)
{
    if (s->len == 0)
        return NULL;
    return s->items[--s->len];    /* ownership leaves the stack */
}

const char *sstack_peek(const sstack *s)
{
    return s->len ? s->items[s->len - 1] : NULL;
}

size_t sstack_size(const sstack *s)
{
    return s->len;
}
