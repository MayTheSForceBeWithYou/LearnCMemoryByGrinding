#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "join.h"

char *str_join(const char *const *parts, size_t n, const char *sep)
{
    size_t lsep = strlen(sep);
    size_t total = 1;                                  /* the NUL */
    for (size_t i = 0; i < n; i++) {
        size_t add = strlen(parts[i]) + (i > 0 ? lsep : 0);
        if (add > SIZE_MAX - total)
            return NULL;
        total += add;
    }
    char *s = malloc(total);
    if (s == NULL)
        return NULL;
    char *p = s;
    for (size_t i = 0; i < n; i++) {
        if (i > 0) {
            memcpy(p, sep, lsep);
            p += lsep;
        }
        size_t len = strlen(parts[i]);
        memcpy(p, parts[i], len);
        p += len;
    }
    *p = '\0';
    return s;
}
