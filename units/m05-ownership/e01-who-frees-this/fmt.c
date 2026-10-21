#include <stdio.h>
#include <stdlib.h>
#include "fmt.h"

const char *fmt_int(int n)
{
    static char buf[16];
    snprintf(buf, sizeof buf, "%d", n);
    return buf;
}

char *fmt_into(char *buf, size_t cap, int n)
{
    snprintf(buf, cap, "%d", n);
    return buf;
}

char *fmt_hex_new(unsigned n)
{
    char *s = malloc(11);           /* "0x" + 8 hex digits + NUL */
    if (s == NULL)
        return NULL;
    snprintf(s, 11, "0x%x", n);
    return s;
}

const char *fmt_version(void)
{
    return "1.4.2";
}
