#ifndef INTLIST_H
#define INTLIST_H
#include <stddef.h>

typedef struct {
    int *vals;  /* owned, room for cap ints */
    size_t len, cap;
} IntList;

/* Parses a comma-separated list of decimal ints such as "4,-2,17" (no spaces,
 * at least one number, each fitting in an int).
 * Returns 0 and stores a new list in *out (free it with intlist_free),
 * -1 on allocation failure, or -2 on malformed input.
 * On -1 and -2, *out is untouched and nothing is leaked. */
int parse_ints(const char *csv, IntList **out);

/* Frees l and its array. Accepts NULL. */
void intlist_free(IntList *l);

#endif
