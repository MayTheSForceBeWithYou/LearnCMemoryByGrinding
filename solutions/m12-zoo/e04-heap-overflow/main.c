/* m12-e04-heap-overflow [BUG] */
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* Joins words[0..n) with sep between them into one heap string.
 * The caller frees it. Returns NULL on allocation failure. */
static char *join(const char *const *words, size_t n, const char *sep)
{
    size_t total = 1;
    for (size_t i = 0; i < n; i++)
        total += strlen(words[i]);
    if (n > 1)
        total += (n - 1) * strlen(sep);

    char *out = malloc(total);
    if (!out)
        return NULL;
    char *p = out;
    size_t seplen = strlen(sep);
    for (size_t i = 0; i < n; i++) {
        if (i > 0) {
            memcpy(p, sep, seplen);
            p += seplen;
        }
        size_t len = strlen(words[i]);
        memcpy(p, words[i], len);
        p += len;
    }
    *p = '\0';
    return out;
}

int main(void)
{
    const char *parts[] = {"usr", "local", "share", "doc"};
    char *path = join(parts, 4, " / ");
    if (!path) {
        fprintf(stderr, "m12-e04: out of memory\n");
        return EXIT_OOM;
    }
    check(strcmp(path, "usr / local / share / doc") == 0, "joined with separators");
    free(path);
    return checks_done("m12-e04-heap-overflow");
}
