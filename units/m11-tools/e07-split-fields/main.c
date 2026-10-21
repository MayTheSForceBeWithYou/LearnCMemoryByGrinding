/* m11-e07-split-fields [FAULT] */
#include <mcheck.h>
#include <string.h>
#include "course.h"
#include "harness.h"

/* Write split_fields() and free_fields() (declared in harness.h) in this file. */

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
