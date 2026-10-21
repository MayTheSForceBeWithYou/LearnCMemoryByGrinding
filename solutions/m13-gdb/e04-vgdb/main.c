/* m13-e04-vgdb [BUG] (graded by Valgrind) */
#include <stdlib.h>
#include "course.h"

enum { HEAVY = 1 << 0 };

typedef struct {
    int id;
    int weight;
    unsigned flags; /* HEAVY when weight > 50 */
} Item;

/* Returns n items: ids 1..n, the given weights, flags set accordingly.
 * The caller frees it. NULL on allocation failure. */
static Item *items_new(const int *weights, size_t n)
{
    Item *it = malloc(n * sizeof *it);
    if (!it)
        return NULL;
    for (size_t i = 0; i < n; i++) {
        it[i].id = (int)i + 1;
        it[i].weight = weights[i];
        it[i].flags = weights[i] > 50 ? HEAVY : 0;
    }
    return it;
}

static size_t count_heavy(const Item *it, size_t n)
{
    size_t k = 0;
    for (size_t i = 0; i < n; i++)
        if (it[i].flags & HEAVY)
            k++;
    return k;
}

int main(void)
{
    const int w[] = {10, 70, 30, 90, 20};
    Item *it = items_new(w, 5);
    if (!it) {
        fprintf(stderr, "m13-e04: out of memory\n");
        return EXIT_OOM;
    }
    check(it[4].id == 5 && it[4].weight == 20, "fields are set");
    check(count_heavy(it, 5) == 2, "two heavy items");
    free(it);
    return checks_done("m13-e04-vgdb");
}
