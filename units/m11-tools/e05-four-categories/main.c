/* m11-e05-four-categories [PREDICT]: write PREDICTION.md before you run anything. */
#include <stdlib.h>
#include <string.h>
#include "course.h"

typedef struct {
    char *payload; /* owned */
} Box;

static char *cache;    /* points at the start of a block */
static char *cursor;   /* points into the middle of a block */

int main(void)
{
    /* 1 */
    cache = malloc(100);
    /* 2 */
    char *scratch = malloc(40);
    /* 3 */
    Box *box = malloc(sizeof *box);
    /* 4 */
    char *ring = malloc(64);
    if (!cache || !scratch || !box || !ring)
        return EXIT_OOM;
    box->payload = malloc(24);
    if (!box->payload)
        return EXIT_OOM;

    memset(scratch, 'x', 40);
    cursor = ring + 10;
    scratch = NULL;
    box = NULL;
    ring = NULL;

    check(cache != NULL && cursor != NULL, "globals are set");
    return checks_done("m11-e05-four-categories");
}
