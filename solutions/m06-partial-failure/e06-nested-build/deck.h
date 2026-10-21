#ifndef DECK_H
#define DECK_H

#include <stddef.h>

struct deck {
    char *title;     /* owned */
    char **cards;    /* owned array of n owned strings */
    size_t n;
};

/* Copies title and cards[0..n-1] (all borrowed). Caller owns the result (deck_destroy).
 * NULL on allocation failure, with nothing leaked.
 * n == 0: cards may be NULL; the result has d->cards == NULL and d->n == 0. */
struct deck *deck_create(const char *title, const char *const *cards, size_t n);
/* Accepts NULL. */
void deck_destroy(struct deck *d);

#endif
