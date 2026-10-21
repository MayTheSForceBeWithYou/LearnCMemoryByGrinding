/* m11-e06-track-origins [REPORT]: read capture.txt before you run anything. */
#include <stdlib.h>
#include "course.h"

typedef struct {
    int width;
    int height;
    int bordered; /* nonzero: area excludes a one-cell border */
} Box;

/* Returns a new w x h box without a border, or NULL on allocation failure. */
static Box *box_new(int w, int h)
{
    Box *b = malloc(sizeof *b);
    if (!b)
        return NULL;
    b->width = w;
    b->height = h;
    b->bordered = 0;
    return b;
}

static int box_area(const Box *b)
{
    if (b->bordered)
        return (b->width - 2) * (b->height - 2);
    return b->width * b->height;
}

int main(void)
{
    Box *b = box_new(4, 5);
    if (!b) {
        fprintf(stderr, "m11-e06: out of memory\n");
        return EXIT_OOM;
    }
    check(box_area(b) == 20, "a 4x5 box without border has area 20");
    free(b);
    return checks_done("m11-e06-track-origins");
}
