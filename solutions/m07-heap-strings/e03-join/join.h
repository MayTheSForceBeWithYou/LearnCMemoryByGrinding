#ifndef JOIN_H
#define JOIN_H

#include <stddef.h>

/* parts[0] sep parts[1] sep ... parts[n-1], as one new string; caller frees.
 * n == 0 gives "" (a heap string, not NULL). parts and sep are borrowed.
 * NULL on allocation failure or if the size would overflow. */
char *str_join(const char *const *parts, size_t n, const char *sep);

#endif
