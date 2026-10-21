#include <stdlib.h>
#include "points.h"

struct pt *points_load(uint32_t n, uint32_t seed)
{
    if (n > POINTS_MAX_BYTES / sizeof(struct pt))   /* check the count, before multiplying */
        return NULL;
    size_t bytes = (size_t)n * sizeof(struct pt);
    struct pt *p = malloc(bytes);
    if (p == NULL)
        return NULL;
    uint32_t s = seed;
    for (uint32_t i = 0; i < n; i++) {
        s = s * 1103515245u + 12345u;
        p[i].x = (int32_t)(s >> 16);
        p[i].y = (int32_t)(s & 0xffff);
        p[i].z = (int32_t)i;
    }
    return p;
}
