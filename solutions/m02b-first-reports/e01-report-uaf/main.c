#include <stdio.h>
#include "course.h"
#include "inventory.h"

int main(void)
{
    struct inventory inv = {0};
    struct item *sword = inventory_add(&inv, "sword", 12);
    struct item *rope = inventory_add(&inv, "rope", 3);
    struct item *lamp = inventory_add(&inv, "lamp", 5);
    if (sword == NULL || rope == NULL || lamp == NULL) {
        inventory_free(&inv);
        fprintf(stderr, "m02b-e01-report-uaf: out of memory\n");
        return EXIT_OOM;
    }
    struct item *heaviest = sword;

    /* The player drops the sword. */
    inventory_drop(&inv, 0);
    heaviest = rope->weight >= lamp->weight ? rope : lamp;   /* re-derive from live items */
    int total = 0;
    for (size_t i = 0; i < inv.count; i++)
        total += inv.slots[i]->weight;
    check(total == 8, "rope + lamp weigh 8");
    check(heaviest->weight <= total, "heaviest item is still carried");

    inventory_free(&inv);
    return checks_done("m02b-e01-report-uaf");
}
