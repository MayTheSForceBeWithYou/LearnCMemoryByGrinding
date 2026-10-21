#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "join.h"

static int is(char *got, const char *want)
{
    int ok = got != NULL && strcmp(got, want) == 0;
    free(got);
    return ok;
}

int main(void)
{
    const char *abc[] = { "a", "b", "c" };
    const char *holes[] = { "", "x", "" };
    check(is(str_join(abc, 3, ", "), "a, b, c"), "basic");
    check(is(str_join(abc, 1, ", "), "a"), "one part, no separator");
    check(is(str_join(abc, 0, ", "), ""), "zero parts: empty string");
    check(is(str_join(abc, 3, ""), "abc"), "empty separator");
    check(is(str_join(holes, 3, "-"), "-x-"), "empty parts");

    enum { N = 10000 };
    static const char *many[N];
    for (int i = 0; i < N; i++)
        many[i] = "word";
    char *big = str_join(many, N, " ");
    if (big == NULL) {
        fprintf(stderr, "m07-e03-join: out of memory\n");
        return EXIT_OOM;
    }
    check(strlen(big) == (size_t)N * 4 + (N - 1), "10000 parts: exact length");
    check(strncmp(big, "word word ", 10) == 0, "10000 parts: content");
    free(big);
    return checks_done("m07-e03-join");
}
