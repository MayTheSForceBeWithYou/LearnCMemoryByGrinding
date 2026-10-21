#ifndef INVENTORY_H
#define INVENTORY_H

#include <stddef.h>

struct item {
    char name[16];
    int weight;
};

/* Owns an array of pointers to heap items. */
struct inventory {
    struct item *slots[8];
    size_t count;
};

/* Adds a new heap item; returns it (borrowed, valid until dropped) or NULL. */
struct item *inventory_add(struct inventory *inv, const char *name, int weight);
/* Frees the item at idx and closes the gap. */
void inventory_drop(struct inventory *inv, size_t idx);
/* Frees every item. */
void inventory_free(struct inventory *inv);

#endif
