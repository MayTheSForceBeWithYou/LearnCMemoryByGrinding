/* m14-e02-misaligned [BUG] */
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"

typedef struct {
    unsigned char *base; /* owned */
    size_t cap, used;
} Arena;

static int arena_init(Arena *a, size_t cap)
{
    a->base = malloc(cap);
    a->cap = a->base ? cap : 0;
    a->used = 0;
    return a->base ? 0 : -1;
}

/* Returns size bytes suitably aligned for any object type, or NULL. */
static void *arena_alloc(Arena *a, size_t size)
{
    const size_t align = alignof(max_align_t);
    if (size == 0 || size > SIZE_MAX - (align - 1))
        return NULL;
    size_t rounded = (size + align - 1) & ~(align - 1);
    if (rounded > a->cap - a->used)
        return NULL;
    void *p = a->base + a->used;
    a->used += rounded;
    return p;
}

static void arena_free(Arena *a)
{
    free(a->base);
    a->base = NULL;
    a->cap = a->used = 0;
}

typedef struct {
    char tag[3];
    double price;
} Item;

int main(void)
{
    Arena a;
    if (arena_init(&a, 4096) != 0) {
        fprintf(stderr, "m14-e02: out of memory\n");
        return EXIT_OOM;
    }
    char *code = arena_alloc(&a, 5);
    Item *item = arena_alloc(&a, sizeof *item);
    if (!code || !item) {
        arena_free(&a);
        return EXIT_OOM;
    }
    memcpy(code, "SKU1", 5);
    item->price = 9.75;
    memcpy(item->tag, "ab", 3);
    check(item->price == 9.75 && strcmp(code, "SKU1") == 0, "values survive");
    arena_free(&a);
    return checks_done("m14-e02-misaligned");
}
