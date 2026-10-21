#include <stdio.h>
#include <stdlib.h>
#include "pos.h"

/* "(-2147483648, -2147483648)" is 26 chars + NUL. */
const char *pos_static(int x, int y)
{
    static char buf[32];

    snprintf(buf, sizeof buf, "(%d, %d)", x, y);
    return buf;
}

char *pos_into(char *dst, size_t cap, int x, int y)
{
    if (cap == 0)
        return NULL;
    int n = snprintf(dst, cap, "(%d, %d)", x, y);
    if (n < 0 || (size_t)n >= cap)
        return NULL;
    return dst;
}

char *pos_heap(int x, int y)
{
    int n = snprintf(NULL, 0, "(%d, %d)", x, y);
    if (n < 0)
        return NULL;
    char *s = malloc((size_t)n + 1);
    if (s == NULL)
        return NULL;
    snprintf(s, (size_t)n + 1, "(%d, %d)", x, y);
    return s;
}
