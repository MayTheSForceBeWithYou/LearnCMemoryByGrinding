/* m12-e09-under-allocation [BUG] */
#include <stdlib.h>
#include "course.h"

typedef struct {
    double x, y;
} Point;

/* Returns n points evenly spaced on the line y = 2x, from x = 0.
 * The caller frees it. NULL on allocation failure. */
static Point *make_points(size_t n)
{
    Point *pts = malloc(n * sizeof pts);
    if (!pts)
        return NULL;
    for (size_t i = 0; i < n; i++) {
        pts[i].x = (double)i;
        pts[i].y = 2.0 * (double)i;
    }
    return pts;
}

int main(void)
{
    Point *p = make_points(6);
    if (!p) {
        fprintf(stderr, "m12-e09: out of memory\n");
        return EXIT_OOM;
    }
    check(p[5].x == 5.0 && p[5].y == 10.0, "last point is (5, 10)");
    free(p);
    return checks_done("m12-e09-under-allocation");
}
