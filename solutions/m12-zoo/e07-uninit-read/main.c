/* m12-e07-uninit-read [BUG] (graded by Valgrind) */
#include <stdlib.h>
#include "course.h"

/* Returns a heap array of nbins counters: bins[k] is how many values v
 * in vals[0..n) have v / width == k. Values past the last bin are ignored.
 * The caller frees it. NULL on allocation failure. */
static size_t *histogram(const int *vals, size_t n, int width, size_t nbins)
{
    size_t *bins = calloc(nbins, sizeof *bins);
    if (!bins)
        return NULL;
    for (size_t i = 0; i < n; i++) {
        size_t k = (size_t)(vals[i] / width);
        if (k < nbins)
            bins[k]++;
    }
    return bins;
}

int main(void)
{
    const int scores[] = {3, 14, 15, 9, 26, 5, 35, 8, 9, 7};
    size_t *h = histogram(scores, 10, 10, 4);
    if (!h) {
        fprintf(stderr, "m12-e07: out of memory\n");
        return EXIT_OOM;
    }
    check(h[0] == 6, "six scores below 10");
    check(h[1] == 2, "two in 10..19");
    check(h[2] == 1, "one in 20..29");
    check(h[3] == 1, "one in 30..39");
    free(h);
    return checks_done("m12-e07-uninit-read");
}
