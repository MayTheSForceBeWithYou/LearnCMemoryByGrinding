#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bbuf.h"

int bbuf_reserve(struct bbuf *b, size_t extra)
{
    if (extra > SIZE_MAX - b->len)
        return -1;                       /* len + extra would wrap */
    size_t need = b->len + extra;
    if (need <= b->cap)
        return 0;
    size_t nc = b->cap ? b->cap : 16;
    while (nc < need) {
        if (nc > SIZE_MAX / 2)
            return -1;
        nc *= 2;
    }
    unsigned char *tmp = realloc(b->data, nc);
    if (tmp == NULL)
        return -1;
    b->data = tmp;
    b->cap = nc;
    return 0;
}

int bbuf_append(struct bbuf *b, const void *src, size_t n)
{
    if (n == 0)
        return 0;
    if (bbuf_reserve(b, n) != 0)
        return -1;
    memcpy(b->data + b->len, src, n);
    b->len += n;
    return 0;
}

/* Returns 0 if cap == len afterwards, or 1 if the shrinking realloc failed and the
 * buffer kept its larger block (still fully valid). Never fails in a way the caller must handle. */
int bbuf_shrink_to_fit(struct bbuf *b)
{
    if (b->len == b->cap)
        return 0;
    if (b->len == 0) {                   /* never realloc(p, 0) */
        free(b->data);
        b->data = NULL;
        b->cap = 0;
        return 0;
    }
    unsigned char *tmp = realloc(b->data, b->len);
    if (tmp == NULL)
        return 1;
    b->data = tmp;
    b->cap = b->len;
    return 0;
}

void bbuf_free(struct bbuf *b)
{
    free(b->data);
    b->data = NULL;
    b->len = b->cap = 0;
}
