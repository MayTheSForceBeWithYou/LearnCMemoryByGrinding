#include <limits.h>
#include <stdlib.h>
#include "course.h"
#include "image.h"

int main(void)
{
    struct image *ims[3] = { NULL, NULL, NULL };
    const int dims[3][2] = { {4, 3}, {1, 1}, {64, 48} };

    for (int i = 0; i < 3; i++) {
        ims[i] = image_create(dims[i][0], dims[i][1]);
        if (ims[i] == NULL) {
            for (int k = 0; k < i; k++)
                image_destroy(ims[k]);
            fprintf(stderr, "m06-e01-two-step: out of memory\n");
            return EXIT_OOM;
        }
    }
    check(ims[0]->w == 4 && ims[0]->h == 3, "dimensions stored");
    int zero = 1;
    for (int i = 0; i < 64 * 48; i++)
        zero &= ims[2]->px[i] == 0;
    check(zero, "pixels start at 0");
    ims[0]->px[11] = 255;
    check(ims[0]->px[11] == 255, "last pixel writable");

    check(image_create(0, 5) == NULL, "zero width rejected");
    check(image_create(5, -1) == NULL, "negative height rejected");
    check(image_create(INT_MAX, INT_MAX) == NULL, "INT_MAX x INT_MAX rejected (int overflow would be UB)");
    check(image_create(65536, 65536) == NULL, "2^32 pixels rejected");
    check(image_create(8192, 8193) == NULL, "just over the limit rejected");
    for (int i = 0; i < 3; i++)
        image_destroy(ims[i]);
    image_destroy(NULL);
    return checks_done("m06-e01-two-step");
}
