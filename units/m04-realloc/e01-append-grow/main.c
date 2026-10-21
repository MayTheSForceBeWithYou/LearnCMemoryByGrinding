#include <stdlib.h>
#include "course.h"
#include "ints.h"

static int fill(struct ints *v, int n)
{
    for (int i = 0; i < n; i++)
        if (ints_push(v, i * 3) != 0)
            return -1;
    return 0;
}

int main(void)
{
    struct ints v = {0};

    if (fill(&v, 100000) != 0) {
        ints_free(&v);
        fprintf(stderr, "m04-e01-append-grow: out of memory\n");
        return EXIT_OOM;
    }
    check(v.len == 100000, "len counts pushes");
    check(v.cap >= v.len, "cap >= len");
    int ok = 1;
    for (size_t i = 0; i < v.len; i++)
        ok &= v.data[i] == (int)i * 3;
    check(ok, "all values kept across every grow");

    ints_free(&v);
    check(v.data == NULL && v.len == 0 && v.cap == 0, "free leaves {0}");
    ints_free(&v);   /* freeing an empty array is fine */

    if (fill(&v, 10) != 0) {
        ints_free(&v);
        fprintf(stderr, "m04-e01-append-grow: out of memory\n");
        return EXIT_OOM;
    }
    check(v.len == 10 && v.data[9] == 27, "reusable after free");
    ints_free(&v);
    return checks_done("m04-e01-append-grow");
}
