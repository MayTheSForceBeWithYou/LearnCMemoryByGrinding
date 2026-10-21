/* m11-e03-return-local [REPORT]: read capture.txt before you run anything. */
#include <stdio.h>
#include <string.h>
#include "course.h"

/* Formats "item-<id>" into buf and returns buf. */
static const char *format_label(char *buf, size_t size, int id)
{
    snprintf(buf, size, "item-%d", id);
    return buf;
}

/* Returns the display label for id. */
static const char *label_for(int id)
{
    char buf[16];
    return format_label(buf, sizeof buf, id);
}

int main(void)
{
    const char *label = label_for(7);
    check(strcmp(label, "item-7") == 0, "label_for(7) is item-7");
    return checks_done("m11-e03-return-local");
}
