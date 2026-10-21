#ifndef SCORES_H
#define SCORES_H

#include <stddef.h>

struct scores {
    char *title;   /* owned */
    int *vals;     /* owned, n entries (NULL when n == 0) */
    size_t n;
};

/* Loads path (first line: title; then one int per line). On success returns 0 and stores
 * a new struct in *out (caller owns: scores_destroy). On any failure returns -1,
 * leaves *out unchanged, and releases everything (memory and the FILE). */
int  scores_load(const char *path, struct scores **out);
/* Accepts NULL. */
void scores_destroy(struct scores *s);

#endif
