#include <stdlib.h>
#include "course.h"
#include "points.h"

int main(int argc, char **argv)
{
    uint32_t n = 1000;
    if (argc > 1)
        n = (uint32_t)strtoul(argv[1], NULL, 10);
    /* Does 12 * n exceed 32 bits? Computed in 64-bit, where it can't wrap. */
    int wraps = (uint64_t)n * sizeof(struct pt) > UINT32_MAX;
    struct pt *p = points_load(n, 42);
    if (wraps) {
        check(p == NULL, "a count whose byte size exceeds 32 bits is refused");
        free(p);
        return checks_done("m03-e02-break-count");
    }
    if (p == NULL) {
        fprintf(stderr, "m03-e02-break-count: points_load(%lu) refused or out of memory\n", (unsigned long)n);
        return EXIT_OOM;
    }
    check(n == 0 || p[n - 1].z == (int32_t)(n - 1), "last point filled");
    free(p);

    return checks_done("m03-e02-break-count");
}
