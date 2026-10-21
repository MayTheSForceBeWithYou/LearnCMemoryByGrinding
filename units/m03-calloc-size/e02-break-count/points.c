#include <stdlib.h>
#include "points.h"

struct pt *points_load(uint32_t n, uint32_t seed)
{
    uint32_t bytes = n * (uint32_t)sizeof(struct pt);
    if (bytes > POINTS_MAX_BYTES)
        return NULL;
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
