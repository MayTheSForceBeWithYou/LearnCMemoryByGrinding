#include <string.h>
#include "course.h"
#include "deck.h"

int main(void)
{
    char src[10][8];
    const char *cards[10];
    for (int i = 0; i < 10; i++) {
        snprintf(src[i], sizeof src[i], "card%d", i);
        cards[i] = src[i];
    }
    struct deck *d = deck_create("Tarot", cards, 10);
    struct deck *e = d ? deck_create("Empty", NULL, 0) : NULL;
    if (d == NULL || e == NULL) {
        deck_destroy(d);
        fprintf(stderr, "m06-e06-nested-build: out of memory\n");
        return EXIT_OOM;
    }
    memset(src, 'X', sizeof src);                  /* the sources were borrowed */
    check(strcmp(d->title, "Tarot") == 0 && d->n == 10, "title and count");
    check(strcmp(d->cards[0], "card0") == 0 && strcmp(d->cards[9], "card9") == 0, "cards copied");
    check(e->n == 0 && strcmp(e->title, "Empty") == 0, "empty deck");
    deck_destroy(e);
    deck_destroy(d);
    deck_destroy(NULL);
    return checks_done("m06-e06-nested-build");
}
