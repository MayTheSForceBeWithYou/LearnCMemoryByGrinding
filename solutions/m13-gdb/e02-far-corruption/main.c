/* m13-e02-far-corruption [BUG] */
#include <stdlib.h>
#include <string.h>
#include "course.h"

typedef struct {
    char name[8];
    size_t count; /* number of ints in items */
    int *items;   /* owned */
} Bag;

/* Gives b count items 1, 2, ..., count. Returns 0, or -1 on allocation failure. */
static int bag_init(Bag *b, size_t count)
{
    b->name[0] = '\0';
    b->count = count;
    b->items = malloc(count * sizeof *b->items);
    if (!b->items)
        return -1;
    for (size_t i = 0; i < count; i++)
        b->items[i] = (int)i + 1;
    return 0;
}

/* Sets the bag's display label, truncated to fit name. */
static void bag_label(Bag *b, const char *label)
{
    snprintf(b->name, sizeof b->name, "%s", label);
}

static long bag_sum(const Bag *b)
{
    long sum = 0;
    for (size_t i = 0; i < b->count; i++)
        sum += b->items[i];
    return sum;
}

int main(void)
{
    Bag b;
    if (bag_init(&b, 3) != 0) {
        fprintf(stderr, "m13-e02: out of memory\n");
        return EXIT_OOM;
    }
    bag_label(&b, "inventory");
    check(b.count == 3, "the bag still has three items");
    check(bag_sum(&b) == 6, "1 + 2 + 3 == 6");
    free(b.items);
    return checks_done("m13-e02-far-corruption");
}
