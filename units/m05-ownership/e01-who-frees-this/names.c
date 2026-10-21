#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "names.h"

const char *names_default(void)
{
    return "nobody";
}

char *names_join(const char *a, const char *b)
{
    size_t la = strlen(a), lb = strlen(b);
    char *s = malloc(la + 1 + lb + 1);
    if (s == NULL)
        return NULL;
    memcpy(s, a, la);
    s[la] = ' ';
    memcpy(s + la + 1, b, lb + 1);
    return s;
}

const char *names_upper(const char *s)
{
    static char buf[32];
    size_t i = 0;
    for (; s[i] != '\0' && i < sizeof buf - 1; i++)
        buf[i] = (char)toupper((unsigned char)s[i]);
    buf[i] = '\0';
    return buf;
}

const char *names_pick(const char *const *list, int i)
{
    return list[i];
}
