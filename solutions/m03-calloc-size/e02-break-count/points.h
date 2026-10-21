#ifndef POINTS_H
#define POINTS_H

#include <stdint.h>

struct pt { int32_t x, y, z; };          /* 12 bytes */

/* The file format never allows more than this many bytes of point data. */
#define POINTS_MAX_BYTES (64u * 1024u * 1024u)

/* Returns n pseudo-random points (caller frees), or NULL if too big or out of memory. */
struct pt *points_load(uint32_t n, uint32_t seed);

#endif
