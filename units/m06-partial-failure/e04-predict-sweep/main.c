/* M06 e04: complete and correct. Predict FA_COUNT and every FAIL_AT outcome first. */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"

struct cache {
    char *name;
    int *slots;      /* nslots, zeroed */
    size_t nslots;
    int *fast;       /* optional lookup table; NULL means "run without it" */
};

static void cache_destroy(struct cache *c)
{
    if (c == NULL)
        return;
    free(c->fast);
    free(c->slots);
    free(c->name);
    free(c);
}

static struct cache *cache_create(const char *name, size_t nslots)
{
    struct cache *c = calloc(1, sizeof *c);
    if (c == NULL)
        return NULL;
    c->name = strdup(name);
    c->slots = calloc(nslots, sizeof *c->slots);
    if (c->name == NULL || c->slots == NULL) {
        cache_destroy(c);
        return NULL;
    }
    c->nslots = nslots;
    c->fast = malloc(nslots * sizeof *c->fast);   /* failure tolerated */
    if (c->fast != NULL)
        for (size_t i = 0; i < nslots; i++)
            c->fast[i] = (int)i;
    return c;
}

static int lookup(const struct cache *c, size_t i)
{
    if (c->fast != NULL)
        return c->fast[i];
    return (int)i;                                  /* the slow path gives the same answer */
}

int main(void)
{
    struct cache *a = cache_create("alpha", 8);
    struct cache *b = a ? cache_create("beta", 32) : NULL;
    if (a == NULL || b == NULL) {
        cache_destroy(a);
        fprintf(stderr, "m06-e04-predict-sweep: out of memory\n");
        return EXIT_OOM;
    }
    check(lookup(a, 5) == 5 && lookup(b, 31) == 31, "lookups agree with or without the fast table");
    check(a->slots[7] == 0 && strcmp(b->name, "beta") == 0, "contents");
    cache_destroy(b);
    cache_destroy(a);
    return checks_done("m06-e04-predict-sweep");
}
