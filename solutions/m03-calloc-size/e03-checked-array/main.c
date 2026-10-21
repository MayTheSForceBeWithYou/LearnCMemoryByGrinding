/* M03 e03: a checked array allocator. Write the three definitions below the marked line.
 * Until you do, the build fails at link time with "undefined reference". */
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* true if a * b does not fit in size_t. No wider types, no builtins. */
bool mul_overflows(size_t a, size_t b);
/* malloc(n * elem), or NULL with errno = ENOMEM if the product overflows. */
void *alloc_array(size_t n, size_t elem);
/* zeroed; same contract as calloc, but built from the two functions above. */
void *alloc_array0(size_t n, size_t elem);

int main(void)
{
    const size_t elems[] = { 1, 3, 8, SIZE_MAX };
    for (size_t i = 0; i < sizeof elems / sizeof elems[0]; i++) {
        size_t e = elems[i];
        size_t max_ok = SIZE_MAX / e;
        check(!mul_overflows(max_ok, e), "largest accepted n does not overflow");
        if (max_ok < SIZE_MAX)   /* for e == 1 nothing overflows */
            check(mul_overflows(max_ok + 1, e), "smallest rejected n overflows");
        check(!mul_overflows(0, e) && !mul_overflows(e, 0), "zero never overflows");
    }
    check(mul_overflows(SIZE_MAX, SIZE_MAX), "SIZE_MAX squared overflows");
    check(!mul_overflows((size_t)1 << 32, ((size_t)1 << 31) - 1), "just below 2^63");
    check(mul_overflows((size_t)1 << 32, (size_t)1 << 32), "2^64 overflows");

    errno = 0;
    check(alloc_array(SIZE_MAX / 2 + 1, 2) == NULL && errno == ENOMEM, "overflow -> NULL, ENOMEM");
    errno = 0;
    /* (SIZE_MAX/8 + 1) * 8 wraps to 0: an unchecked malloc(0) would succeed. */
    check(alloc_array(SIZE_MAX / 8 + 1, 8) == NULL && errno == ENOMEM, "product wrapping to 0 -> NULL, ENOMEM");
    errno = 0;
    /* PTRDIFF_MAX/8 * 8 fits (about 8 EiB) but no allocator can provide it: malloc itself
     * fails with ENOMEM. (Sizes above PTRDIFF_MAX are rejected too, but Valgrind reports
     * them as a "fishy" argument, so the harness stays below.) */
    check(alloc_array(PTRDIFF_MAX / 8, 8) == NULL && errno == ENOMEM, "impossible size -> NULL, ENOMEM");
    errno = 0;
    check(alloc_array0(SIZE_MAX, SIZE_MAX) == NULL && errno == ENOMEM, "overflow -> NULL, ENOMEM (zeroed)");

    int *v = alloc_array(100, sizeof *v);
    unsigned char *z = alloc_array0(64, 3);
    if (v == NULL || z == NULL) {
        free(v);
        free(z);
        fprintf(stderr, "m03-e03-checked-array: out of memory\n");
        return EXIT_OOM;
    }
    v[99] = 7;
    check(v[99] == 7, "alloc_array gives usable memory");
    size_t nz = 0;
    for (size_t i = 0; i < 64 * 3; i++)
        nz += z[i] != 0;
    check(nz == 0, "alloc_array0 zeroes every byte");
    free(v);
    free(z);
    free(alloc_array(0, 8));
    free(alloc_array0(8, 0));
    return checks_done("m03-e03-checked-array");
}

/* ---- your definitions below ---- */

bool mul_overflows(size_t a, size_t b)
{
    return b != 0 && a > SIZE_MAX / b;
}

void *alloc_array(size_t n, size_t elem)
{
    if (mul_overflows(n, elem)) {
        errno = ENOMEM;
        return NULL;
    }
    return malloc(n * elem);
}

void *alloc_array0(size_t n, size_t elem)
{
    void *p = alloc_array(n, elem);   /* sets errno on overflow */
    if (p != NULL)
        memset(p, 0, n * elem);       /* safe: the product was checked */
    return p;
}
