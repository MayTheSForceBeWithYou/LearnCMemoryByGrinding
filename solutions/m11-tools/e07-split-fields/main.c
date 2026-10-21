/* m11-e07-split-fields [FAULT]: reference solution. */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <mcheck.h>
#include <string.h>
#include "course.h"
#include "harness.h"


char **split_fields(const char *line, char sep, size_t *count)
{
    size_t n = 1;
    for (const char *p = line; *p; p++)
        if (*p == sep)
            n++;

    char **fields = malloc((n + 1) * sizeof *fields);
    if (!fields)
        return NULL;

    size_t i = 0;
    const char *start = line;
    for (;;) {
        const char *end = strchr(start, sep);
        size_t len = end ? (size_t)(end - start) : strlen(start);
        fields[i] = strndup(start, len);
        if (!fields[i])
            goto fail;
        i++;
        if (!end)
            break;
        start = end + 1;
    }
    fields[i] = NULL;
    *count = n;
    return fields;

fail:
    while (i > 0)
        free(fields[--i]);
    free(fields);
    return NULL;
}

void free_fields(char **fields)
{
    if (!fields)
        return;
    for (char **p = fields; *p; p++)
        free(*p);
    free(fields);
}

int main(void)
{
    mtrace(); /* does nothing unless MALLOC_TRACE is set; see README */

    size_t n = 99;
    char **f = split_fields("id,name,,score", ',', &n);
    if (!f)
        goto oom;
    check(n == 4, "four fields");
    if (n == 4) {
        check(strcmp(f[0], "id") == 0, "field 0");
        check(strcmp(f[1], "name") == 0, "field 1");
        check(strcmp(f[2], "") == 0, "field 2 is empty");
        check(strcmp(f[3], "score") == 0, "field 3");
    }
    check(f[n] == NULL, "array is NULL-terminated");
    free_fields(f);

    f = split_fields("", ';', &n);
    if (!f)
        goto oom;
    check(n == 1 && f[0][0] == '\0' && f[1] == NULL, "empty line gives one empty field");
    free_fields(f);
    free_fields(NULL);

    muntrace();
    return checks_done("m11-e07-split-fields");
oom:
    muntrace();
    fprintf(stderr, "m11-e07: out of memory\n");
    return EXIT_OOM;
}
