#ifndef BBUF_H
#define BBUF_H

#include <stddef.h>

/* A growable byte buffer. {0} is valid and empty. Owns data. */
struct bbuf {
    unsigned char *data;
    size_t len, cap;
};

/* Ensures cap >= len + extra. 0, or -1 on overflow/OOM with *b unchanged. */
int bbuf_reserve(struct bbuf *b, size_t extra);
/* Appends n bytes from src (src may be NULL when n == 0). 0, or -1 with *b unchanged. */
int bbuf_append(struct bbuf *b, const void *src, size_t n);
/* Makes cap == len. An empty buffer is freed (data = NULL). Return value: document it. */
int bbuf_shrink_to_fit(struct bbuf *b);
/* Frees and resets to {0}. */
void bbuf_free(struct bbuf *b);

#endif
