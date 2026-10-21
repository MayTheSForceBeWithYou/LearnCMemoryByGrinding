#define _POSIX_C_SOURCE 200809L   /* strdup is POSIX, not ISO C */
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "path.h"

static int is(char *got, const char *want)
{
    if (got == NULL) {
        fprintf(stderr, "m07-e06-design-paths: out of memory\n");
        exit(EXIT_OOM);
    }
    int ok = strcmp(got, want) == 0;
    free(got);
    return ok;
}

int main(void)
{
    check(is(path_join("a/b", "c.txt"), "a/b/c.txt"), "join");
    check(is(path_join("a/b/", "c.txt"), "a/b/c.txt"), "join, trailing slash");
    check(is(path_join("", "c.txt"), "c.txt"), "join, empty dir");
    check(is(path_join("/", "etc"), "/etc"), "join, root");

    check(strcmp(path_basename("a/b/c.txt"), "c.txt") == 0, "basename");
    check(strcmp(path_basename("c.txt"), "c.txt") == 0, "basename, no slash");
    const char *lit = "x/y";
    check(path_basename(lit) == lit + 2, "basename points into the argument");

    check(is(path_dirname("a/b/c.txt"), "a/b"), "dirname");
    check(is(path_dirname("c.txt"), "."), "dirname, no slash");
    check(is(path_dirname("/x"), "/"), "dirname, root");

    check(is(path_with_ext("a/b.tar", ".gz"), "a/b.gz"), "with_ext, replace");
    check(is(path_with_ext("a/b", ".gz"), "a/b.gz"), "with_ext, append");
    check(is(path_with_ext("v1.2/readme", ".md"), "v1.2/readme.md"), "with_ext, dotted dir");

    char *owned = strdup("logs/today.txt");
    if (owned == NULL) {
        fprintf(stderr, "m07-e06-design-paths: out of memory\n");
        return EXIT_OOM;
    }
    const char *base = path_basename(owned);
    check(strcmp(base, "today.txt") == 0, "basename of heap string");
    free(owned);            /* base is now dangling: its lifetime was owned's */
    base = NULL;
    return checks_done("m07-e06-design-paths");
}
