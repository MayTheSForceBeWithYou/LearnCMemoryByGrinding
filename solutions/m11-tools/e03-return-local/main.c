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

/* Writes the display label for id into the caller's buf and returns buf. */
static const char *label_for(char *buf, size_t size, int id)
{
    return format_label(buf, size, id);
}

int main(void)
{
    char buf[16];
    const char *label = label_for(buf, sizeof buf, 7);
    check(strcmp(label, "item-7") == 0, "label_for(7) is item-7");
    return checks_done("m11-e03-return-local");
}
