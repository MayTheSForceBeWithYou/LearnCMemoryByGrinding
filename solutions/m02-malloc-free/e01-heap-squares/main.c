/* M02 e01: heap squares. Write the two definitions below the marked line.
 * Until you do, the build fails at link time with "undefined reference". */
#include <stdlib.h>
#include "course.h"

/* Returns a new heap array {0, 1, 4, ..., (n-1)^2}; caller frees (or passes it to
 * sum_and_free). NULL on allocation failure. n == 0: see your comment on the definition. */
long *squares(size_t n);

/* Sums v[0..n-1], then frees v. Accepts NULL when n == 0. */
long sum_and_free(long *v, size_t n);

static int oom(void)
{
    fprintf(stderr, "m02-e01-heap-squares: out of memory\n");
    return EXIT_OOM;
}

int main(void)
{
    long *v = squares(5);
    if (v == NULL)
        return oom();
    check(v[0] == 0 && v[1] == 1 && v[4] == 16, "squares(5) values");
    check(sum_and_free(v, 5) == 30, "sum of squares 0..4 is 30");

    check(sum_and_free(squares(0), 0) == 0, "n == 0 works either way");
    check(sum_and_free(NULL, 0) == 0, "sum_and_free accepts NULL with n == 0");

    v = squares(1);
    if (v == NULL)
        return oom();
    check(v[0] == 0, "squares(1)");
    sum_and_free(v, 1);

    size_t n = 100000;
    v = squares(n);
    if (v == NULL)
        return oom();
    check(v[n - 1] == (long)(n - 1) * (long)(n - 1), "last of 100000");
    long expect = (long)(n - 1) * (long)n * (long)(2 * n - 1) / 6;
    check(sum_and_free(v, n) == expect, "sum of 100000 squares");
    return checks_done("m02-e01-heap-squares");
}

/* ---- your definitions below ---- */

/* n == 0: returns NULL without calling malloc. That is unambiguous, and
 * sum_and_free(NULL, 0) and free(NULL) are both fine. */
long *squares(size_t n)
{
    if (n == 0)
        return NULL;
    long *v = malloc(n * sizeof *v);
    if (v == NULL)
        return NULL;
    for (size_t i = 0; i < n; i++)
        v[i] = (long)i * (long)i;
    return v;
}

long sum_and_free(long *v, size_t n)
{
    long sum = 0;

    for (size_t i = 0; i < n; i++)
        sum += v[i];
    free(v);
    return sum;
}
