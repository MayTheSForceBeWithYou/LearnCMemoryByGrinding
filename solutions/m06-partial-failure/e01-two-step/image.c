#include <stdint.h>
#include <stdlib.h>
#include "image.h"

struct image *image_create(int w, int h)
{
    if (!(w > 0 && h > 0) || w > IMAGE_MAX_PIXELS / h)   /* division form: no overflow */
        return NULL;
    struct image *im = malloc(sizeof *im);
    if (im == NULL)
        return NULL;
    im->px = calloc((size_t)w * (size_t)h, 1);
    if (im->px == NULL) {
        free(im);                                     /* step 2 failed: undo step 1 */
        return NULL;
    }
    im->w = w;
    im->h = h;
    return im;
}

void image_destroy(struct image *im)
{
    if (im == NULL)
        return;
    free(im->px);
    free(im);
}
