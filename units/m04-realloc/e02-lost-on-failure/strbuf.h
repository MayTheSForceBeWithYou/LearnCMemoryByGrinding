#ifndef STRBUF_H
#define STRBUF_H

#include <stddef.h>

/* A growable NUL-terminated string. {0} is valid and empty. Owns data. */
struct strbuf {
    char *data;
    size_t len;   /* excluding the NUL */
    size_t cap;   /* bytes allocated */
};

/* Appends s. 0 on success. -1 on allocation failure, with *sb exactly as before. */
int sb_append(struct strbuf *sb, const char *s);
/* Frees and resets to {0}. */
void sb_free(struct strbuf *sb);

#endif
