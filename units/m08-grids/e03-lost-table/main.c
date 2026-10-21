/* m08-e03-lost-table: three structures, three ways to lose them. Predict first. */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"

static const char *const names[] = {"north", "south", "east", "west"};

/* A table of n row strings (each a strdup of names[i % 4]). */
static char **make_rows(size_t n)
{
    char **t = calloc(n, sizeof *t);
    if (!t)
        return NULL;
    for (size_t i = 0; i < n; i++)
        if (!(t[i] = strdup(names[i % 4])))
            return NULL;   /* not cleanup-safe; this program is about leaks, not OOM */
    return t;
}

static size_t case_a(void)          /* A: 4 rows, table pointer goes out of scope */
{
    char **t = make_rows(4);
    return t ? strlen(t[0]) : 0;
}

static size_t case_b(void)          /* B: flat 8 x 4 grid, pointer overwritten */
{
    char *g = malloc(8 * 4);
    if (!g)
        return 0;
    memset(g, '.', 8 * 4);
    g = NULL;
    return 32;
}

static size_t case_c(void)          /* C: 3 rows, table freed, rows not */
{
    char **t = make_rows(3);
    if (!t)
        return 0;
    size_t len = strlen(t[2]);
    free(t);
    return len;
}

int main(void)
{
    check(case_a() == 5, "A ran");
    check(case_b() == 32, "B ran");
    check(case_c() == 4, "C ran");
    return checks_done("m08-e03-lost-table");
}
