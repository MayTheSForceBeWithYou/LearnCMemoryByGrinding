#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "deck.h"

struct deck *deck_create(const char *title, const char *const *cards, size_t n)
{
    struct deck *d = calloc(1, sizeof *d);
    if (d == NULL)
        return NULL;
    if ((d->title = strdup(title)) == NULL)
        goto fail;
    if (n > 0) {
        d->cards = calloc(n, sizeof *d->cards);   /* zeroed: unfilled slots are NULL */
        if (d->cards == NULL)
            goto fail;
        d->n = n;                                 /* destroy will walk all n slots */
        for (size_t i = 0; i < n; i++)
            if ((d->cards[i] = strdup(cards[i])) == NULL)
                goto fail;
    }
    return d;

fail:
    deck_destroy(d);
    return NULL;
}

void deck_destroy(struct deck *d)
{
    if (d == NULL)
        return;
    for (size_t i = 0; i < d->n; i++)
        free(d->cards[i]);          /* NULL for slots never filled */
    free(d->cards);
    free(d->title);
    free(d);
}
