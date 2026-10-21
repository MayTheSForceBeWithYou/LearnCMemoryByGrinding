/* M02 e03: the stale copy. */
#include <stdlib.h>
#include "course.h"

struct board {
    const int *view;   /* the scores being displayed */
    size_t n;
};

/* Frees old (if any) and returns a fresh array of n scores, all set to start. */
static int *reset_scores(int *old, size_t n, int start)
{
    free(old);
    int *s = malloc(n * sizeof *s);
    if (s == NULL)
        return NULL;
    for (size_t i = 0; i < n; i++)
        s[i] = start;
    return s;
}

static long board_total(const struct board *b)
{
    long t = 0;
    for (size_t i = 0; i < b->n; i++)
        t += b->view[i];
    return t;
}

int main(void)
{
    size_t n = 8;
    int *scores = reset_scores(NULL, n, 0);
    if (scores == NULL) {
        fprintf(stderr, "m02-e03-use-after-free: out of memory\n");
        return EXIT_OOM;
    }
    struct board b = { scores, n };
    for (size_t i = 0; i < n; i++)
        scores[i] = (int)i * 10;
    check(board_total(&b) == 280, "board shows the first round");

    scores = reset_scores(scores, n, 5);
    if (scores == NULL) {
        fprintf(stderr, "m02-e03-use-after-free: out of memory\n");
        return EXIT_OOM;
    }

    check(board_total(&b) == 40, "board shows the reset round");

    free(scores);
    return checks_done("m02-e03-use-after-free");
}
