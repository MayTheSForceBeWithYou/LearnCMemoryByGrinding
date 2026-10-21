/* m12-e10-glibc-aborts [BUG] [NO-ASAN]: run with `make run`, not under a sanitizer. */
#include <stdlib.h>
#include <string.h>
#include "course.h"

typedef struct {
    char *buf; /* owned, or NULL once closed */
    size_t len;
} Reader;

static int reader_open(Reader *r, const char *text)
{
    r->len = strlen(text);
    r->buf = malloc(r->len + 1);
    if (!r->buf)
        return -1;
    memcpy(r->buf, text, r->len + 1);
    return 0;
}

/* Releases the reader's buffer. Safe to call more than once. */
static void reader_close(Reader *r)
{
    free(r->buf);
    r->buf = NULL;
}

/* Returns a heap copy of s padded with spaces on the right to width
 * characters, or the whole of s if it is longer. The caller frees it. */
static char *pad_right(const char *s, size_t width)
{
    size_t len = strlen(s);
    char *out = malloc((width > len ? width : len) + 1);
    if (!out)
        return NULL;
    memcpy(out, s, len + 1);
    for (size_t i = len; i < width; i++)
        out[i] = ' ';
    out[width > len ? width : len] = '\0';
    return out;
}

int main(void)
{
    /* Part A */
    Reader r;
    if (reader_open(&r, "header line") != 0)
        return EXIT_OOM;
    check(r.len == 11, "reader sees 11 bytes");
    reader_close(&r);
    reader_close(&r); /* the cleanup path closes again, as documented */

    /* Part B */
    char *cell = pad_right("this-label-is-much-longer-than-its-column-and-then-some-more", 40);
    if (!cell)
        return EXIT_OOM;
    char *wide = pad_right("x", 2000);
    if (!wide) {
        free(cell);
        return EXIT_OOM;
    }
    check(strcmp(cell, "this-label-is-much-longer-than-its-column-and-then-some-more") == 0, "long labels are kept whole");
    check(strlen(wide) == 2000, "short labels are padded");
    free(wide);
    free(cell);
    return checks_done("m12-e10-glibc-aborts");
}
