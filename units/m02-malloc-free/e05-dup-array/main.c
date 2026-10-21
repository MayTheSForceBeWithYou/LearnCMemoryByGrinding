/* M02 e05: copy what you borrowed. Write dup_ints below the marked line.
 * Until you do, the build fails at link time with "undefined reference". */
#include <stdlib.h>
#include <string.h>
#include "course.h"

int *dup_ints(const int *src, size_t n);

int main(void)
{
    int *src = malloc(5 * sizeof *src);
    if (src == NULL) {
        fprintf(stderr, "m02-e05-dup-array: out of memory\n");
        return EXIT_OOM;
    }
    for (int i = 0; i < 5; i++)
        src[i] = 100 + i;

    int *copy = dup_ints(src, 5);
    if (copy == NULL) {
        free(src);
        fprintf(stderr, "m02-e05-dup-array: out of memory\n");
        return EXIT_OOM;
    }
    check(copy != src, "the copy is a different block");
    check(memcmp(copy, src, 5 * sizeof *src) == 0, "same values");
    src[0] = -1;
    check(copy[0] == 100, "changing the source does not change the copy");
    free(src);
    check(copy[4] == 104, "the copy outlives the source");
    free(copy);

    int stack_src[3] = {7, 8, 9};
    int *c2 = dup_ints(stack_src, 3);
    if (c2 == NULL) {
        fprintf(stderr, "m02-e05-dup-array: out of memory\n");
        return EXIT_OOM;
    }
    check(c2[2] == 9, "works for a stack source too");
    free(c2);

    free(dup_ints(NULL, 0));  /* must not dereference NULL; result may be NULL or not */
    return checks_done("m02-e05-dup-array");
}

/* ---- your definition below ---- */
