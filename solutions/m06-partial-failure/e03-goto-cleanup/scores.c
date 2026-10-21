#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scores.h"

#define LINE_CAP 128

static int parse_int(const char *s, int *out)
{
    char *end;
    errno = 0;
    long v = strtol(s, &end, 10);
    if (end == s || errno != 0 || v < -2147483647L - 1 || v > 2147483647L)
        return -1;
    while (*end == ' ' || *end == '\t')
        end++;
    if (*end != '\0')
        return -1;
    *out = (int)v;
    return 0;
}

/* Strips the newline; -1 if the line was too long (no newline and not at EOF). */
static int chomp(char *line, FILE *f)
{
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
        return 0;
    }
    return feof(f) ? 0 : -1;
}

int scores_load(const char *path, struct scores **out)
{
    int rc = -1;
    FILE *f = NULL;
    char *line = NULL;
    char *title = NULL;
    int *vals = NULL;
    size_t n = 0, cap = 0;
    struct scores *s = NULL;

    f = fopen(path, "r");
    if (f == NULL)
        goto cleanup;
    line = malloc(LINE_CAP);
    if (line == NULL)
        goto cleanup;
    if (fgets(line, LINE_CAP, f) == NULL || chomp(line, f) != 0)
        goto cleanup;
    title = strdup(line);
    if (title == NULL)
        goto cleanup;

    while (fgets(line, LINE_CAP, f) != NULL) {
        int v;
        if (chomp(line, f) != 0 || parse_int(line, &v) != 0)
            goto cleanup;
        if (n == cap) {
            size_t nc = cap ? cap * 2 : 4;
            if (nc > SIZE_MAX / sizeof *vals)
                goto cleanup;
            int *tmp = realloc(vals, nc * sizeof *tmp);
            if (tmp == NULL)
                goto cleanup;
            vals = tmp;
            cap = nc;
        }
        vals[n++] = v;
    }
    if (ferror(f))
        goto cleanup;

    s = malloc(sizeof *s);
    if (s == NULL)
        goto cleanup;
    s->title = title;  title = NULL;   /* ownership moves into the result */
    s->vals = vals;    vals = NULL;
    s->n = n;
    *out = s;          s = NULL;       /* ... and the result moves to the caller */
    rc = 0;

cleanup:
    free(s);
    free(vals);
    free(title);
    free(line);
    if (f != NULL)
        fclose(f);
    return rc;
}

void scores_destroy(struct scores *s)
{
    if (s == NULL)
        return;
    free(s->vals);
    free(s->title);
    free(s);
}
