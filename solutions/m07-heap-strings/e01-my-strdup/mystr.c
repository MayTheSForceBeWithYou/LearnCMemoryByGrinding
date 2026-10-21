#include <stdlib.h>
#include <string.h>
#include "mystr.h"

char *my_strdup(const char *s)
{
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d == NULL)
        return NULL;
    memcpy(d, s, len + 1);          /* includes the NUL */
    return d;
}

char *my_strndup(const char *s, size_t n)
{
    size_t len = 0;
    while (len < n && s[len] != '\0')   /* bounded: never touches s[n] */
        len++;
    char *d = malloc(len + 1);
    if (d == NULL)
        return NULL;
    memcpy(d, s, len);
    d[len] = '\0';
    return d;
}
