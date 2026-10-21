#include <stdio.h>
#include <stdlib.h>
#include "inventory.h"

struct item *inventory_add(struct inventory *inv, const char *name, int weight)
{
    if (inv->count == sizeof inv->slots / sizeof inv->slots[0])
        return NULL;
    struct item *it = malloc(sizeof *it);
    if (it == NULL)
        return NULL;
    snprintf(it->name, sizeof it->name, "%s", name);
    it->weight = weight;
    inv->slots[inv->count++] = it;
    return it;
}

void inventory_drop(struct inventory *inv, size_t idx)
{
    free(inv->slots[idx]);
    for (size_t i = idx + 1; i < inv->count; i++)
        inv->slots[i - 1] = inv->slots[i];
    inv->count--;
}

void inventory_free(struct inventory *inv)
{
    for (size_t i = 0; i < inv->count; i++)
        free(inv->slots[i]);
    inv->count = 0;
}
