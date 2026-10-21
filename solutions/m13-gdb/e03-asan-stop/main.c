/* m13-e03-asan-stop [FAULT]: reference solution. */
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include "course.h"
#include "intlist.h"


/* Appends x, doubling the capacity when full. 0 or -1 (list unchanged). */
static int push(IntList *l, int x)
{
    if (l->len == l->cap) {
        size_t ncap = l->cap ? l->cap * 2 : 2;
        int *p = realloc(l->vals, ncap * sizeof *p);
        if (!p)
            return -1;
        l->vals = p;
        l->cap = ncap;
    }
    l->vals[l->len++] = x;
    return 0;
}

int parse_ints(const char *csv, IntList **out)
{
    IntList *l = malloc(sizeof *l);
    if (!l)
        return -1;
    l->vals = NULL;
    l->len = l->cap = 0;

    int rc = -2;
    const char *p = csv;
    for (;;) {
        char *end;
        errno = 0;
        long v = strtol(p, &end, 10);
        if (end == p || errno == ERANGE || v < INT_MIN || v > INT_MAX)
            goto fail;
        if (*end != ',' && *end != '\0')
            goto fail;
        if (push(l, (int)v) != 0) {
            rc = -1;
            goto fail;
        }
        if (*end == '\0')
            break;
        p = end + 1;
    }
    *out = l;
    return 0;

fail:
    intlist_free(l);
    return rc;
}

void intlist_free(IntList *l)
{
    if (!l)
        return;
    free(l->vals);
    free(l);
}

int main(void)
{
    IntList *l = NULL;
    int rc = parse_ints("4,-2,17,0,99,1000000,-7,3,8", &l);
    if (rc == -1)
        goto oom;
    check(rc == 0, "a valid list parses");
    if (rc == 0) {
        check(l->len == 9, "nine numbers");
        check(l->len == 9 && l->vals[1] == -2 && l->vals[5] == 1000000 && l->vals[8] == 8, "values");
        intlist_free(l);
    }

    const char *bad[] = {"", "1,,2", "1,2,", "x", "12a", "99999999999"};
    for (int i = 0; i < 6; i++) {
        l = NULL;
        rc = parse_ints(bad[i], &l);
        if (rc == -1)
            goto oom;
        check(rc == -2 && l == NULL, "malformed input is rejected");
    }
    intlist_free(NULL);
    return checks_done("m13-e03-asan-stop");
oom:
    fprintf(stderr, "m13-e03: out of memory\n");
    return EXIT_OOM;
}
