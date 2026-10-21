#ifndef POS_H
#define POS_H

#include <stddef.h>

/* Formats "(x, y)".
 * Lifetime/ownership: TODO (what does a second call do to the first result?) */
const char *pos_static(int x, int y);

/* Formats "(x, y)" into dst, writing at most cap bytes including the NUL.
 * Lifetime/ownership: TODO
 * Returns: TODO (fits / does not fit / cap == 0) */
char *pos_into(char *dst, size_t cap, int x, int y);

/* Formats "(x, y)" into a new heap block sized exactly.
 * Lifetime/ownership: TODO (who frees?)
 * Returns: TODO (on allocation failure?) */
char *pos_heap(int x, int y);

#endif
