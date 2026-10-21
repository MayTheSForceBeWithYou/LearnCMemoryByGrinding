#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fmt.h"

char *fmt_alloc(const char *fmt, ...)
{
    va_list ap, ap2;
    va_start(ap, fmt);
    va_copy(ap2, ap);                         /* one va_list per traversal */
    int n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (n < 0) {
        va_end(ap2);
        return NULL;
    }
    char *s = malloc((size_t)n + 1);
    if (s != NULL)
        vsnprintf(s, (size_t)n + 1, fmt, ap2);
    va_end(ap2);
    return s;
}

char *report_build(const struct player *ps, size_t n)
{
    char *out = NULL;
    char **lines = NULL;
    size_t total = 1, built = 0;

    if (n > SIZE_MAX / sizeof *lines)
        return NULL;
    lines = calloc(n ? n : 1, sizeof *lines);
    if (lines == NULL)
        return NULL;
    for (; built < n; built++) {
        lines[built] = fmt_alloc("%s: %d/%d HP\n", ps[built].name, ps[built].hp, ps[built].max);
        if (lines[built] == NULL)
            goto cleanup;
        total += strlen(lines[built]);
    }
    out = malloc(total);
    if (out == NULL)
        goto cleanup;
    char *p = out;
    for (size_t i = 0; i < n; i++) {
        size_t len = strlen(lines[i]);
        memcpy(p, lines[i], len);
        p += len;
    }
    *p = '\0';

cleanup:
    for (size_t i = 0; i < built; i++)        /* frees exactly the lines that exist */
        free(lines[i]);
    free(lines);
    return out;                               /* NULL on any failure */
}
