/* M01 e05: legal C, clean under every tool, and wrong. Explain it in ANSWER.md. */
#include <stdio.h>
#include "course.h"

static const char *const names[] = { "sword", "shield", "potion", "lantern" };

const char *item_name(int id);

const char *item_name(int id)
{
    static char buf[32];

    snprintf(buf, sizeof buf, "#%d %s", id, names[id % 4]);
    return buf;
}

int main(void)
{
    int a = 1, b = 2;

    printf("swap %s <-> %s\n", item_name(a), item_name(b));

    const char *bag[4];
    for (int i = 0; i < 4; i++)
        bag[i] = item_name(i);
    for (int i = 0; i < 4; i++)
        printf("bag[%d] = %s\n", i, bag[i]);

    check(bag[0] == bag[3], "every element holds the same address");
    return checks_done("m01-e05-static-overwrite");
}
