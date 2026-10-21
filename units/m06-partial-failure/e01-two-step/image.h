#ifndef IMAGE_H
#define IMAGE_H

struct image { int w, h; unsigned char *px; };   /* px: w * h bytes, zeroed */

#define IMAGE_MAX_PIXELS (1L << 26)

/* New w x h image, all pixels 0; caller owns it (image_destroy).
 * NULL if !(w > 0 && h > 0), if w * h > IMAGE_MAX_PIXELS (compute this without overflow),
 * or on allocation failure (nothing leaked). */
struct image *image_create(int w, int h);
/* Frees im and its pixels. Accepts NULL. */
void image_destroy(struct image *im);

#endif
