/* m13-e03-asan-stop [FAULT] */
#include "course.h"
#include "intlist.h"

/* Write parse_ints() and intlist_free() (declared in intlist.h) in this file. */

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
