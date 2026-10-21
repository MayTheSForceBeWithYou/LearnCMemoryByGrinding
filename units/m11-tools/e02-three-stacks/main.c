/* m11-e02-three-stacks [REPORT]: read capture.txt before you run anything. */
#include <stdlib.h>
#include <string.h>
#include "course.h"

typedef struct {
    char *buf; /* owned, NUL-terminated, or NULL when empty */
    size_t len;
} Line;

/* Replaces the contents of l with a copy of s. Returns 0, or -1 on
 * allocation failure with l unchanged. */
static int line_set(Line *l, const char *s)
{
    size_t n = strlen(s);
    char *p = realloc(l->buf, n + 1);
    if (!p)
        return -1;
    memcpy(p, s, n + 1);
    l->buf = p;
    l->len = n;
    return 0;
}

static void line_clear(Line *l)
{
    free(l->buf);
    l->buf = NULL;
    l->len = 0;
}

int main(void)
{
    Line l = {NULL, 0};
    if (line_set(&l, "short") != 0)
        goto oom;
    const char *first = l.buf;
    if (line_set(&l, "a considerably longer replacement line") != 0)
        goto oom;
    check(l.len == 38, "length follows the new contents");
    check(first[0] == 's', "the first contents started with 's'");
    line_clear(&l);
    return checks_done("m11-e02-three-stacks");
oom:
    line_clear(&l);
    fprintf(stderr, "m11-e02: out of memory\n");
    return EXIT_OOM;
}
