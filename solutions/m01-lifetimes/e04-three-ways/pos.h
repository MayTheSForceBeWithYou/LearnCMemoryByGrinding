#ifndef POS_H
#define POS_H

#include <stddef.h>

/* Formats "(x, y)".
 * Lifetime/ownership: returns a pointer to one static buffer owned by pos.c.
 * Always valid, never free it. The NEXT call overwrites it, so copy it out if you need
 * two results at once. Not thread-safe. */
const char *pos_static(int x, int y);

/* Formats "(x, y)" into dst, writing at most cap bytes including the NUL.
 * Lifetime/ownership: the caller owns dst and chooses its lifetime. Nothing is allocated.
 * Returns: dst if the whole text fit. NULL if it did not (dst then holds a truncated,
 * NUL-terminated string) or if cap == 0 (nothing written). */
char *pos_into(char *dst, size_t cap, int x, int y);

/* Formats "(x, y)" into a new heap block of exactly strlen + 1 bytes.
 * Lifetime/ownership: the caller owns the result and must free() it.
 * Returns: NULL on allocation failure. */
char *pos_heap(int x, int y);

#endif
