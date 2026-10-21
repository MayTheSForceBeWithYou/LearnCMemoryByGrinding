#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "label.h"

static size_t digits(int v)
{
    size_t d = 1;
    while (v >= 10) {
        v /= 10;
        d++;
    }
    return d;
}

char *label_make(const char *name, int level)
{
    /* "[Lv " + digits + "] " + name + "!" + NUL */
    size_t size = strlen(name) + digits(level) + 8;
    char *s = malloc(size);
    if (s == NULL)
        return NULL;
    sprintf(s, "[Lv %d] %s!", level, name);
    return s;
}

char *label_upper(const char *name, int level)
{
    char *s = label_make(name, level);
    if (s == NULL)
        return NULL;
    char *p = strchr(s, ']') + 2;
    for (; *p != '\0'; p++)
        *p = (char)toupper((unsigned char)*p);
    return s;
}
