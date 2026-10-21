#ifndef MYSTR_H
#define MYSTR_H

#include <stddef.h>

/* New heap copy of s; caller frees. NULL on allocation failure. */
char *my_strdup(const char *s);
/* New heap copy of at most n chars of s, always NUL-terminated; caller frees.
 * Reads no byte at or beyond s[n]. NULL on allocation failure. */
char *my_strndup(const char *s, size_t n);

#endif
