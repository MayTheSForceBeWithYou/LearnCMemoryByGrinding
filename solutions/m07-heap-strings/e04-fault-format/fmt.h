#ifndef FMT_H
#define FMT_H

#include <stddef.h>

struct player { const char *name; int hp, max; };

/* printf-style formatting into a new exactly-sized string; caller frees. NULL on failure. */
char *fmt_alloc(const char *fmt, ...);
/* One line per player: "<name>: <hp>/<max> HP\n". New string; caller frees.
 * n == 0 gives "". NULL on allocation failure, with nothing leaked. */
char *report_build(const struct player *ps, size_t n);

#endif
