/* m12-e06-off-by-one-nul [BUG] */
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* Returns a heap copy of the first n bytes of s, NUL-terminated
 * (n must not exceed strlen(s)). The caller frees it. NULL on failure. */
static char *prefix(const char *s, size_t n)
{
    char *out = malloc(n);
    if (!out)
        return NULL;
    memcpy(out, s, n);
    out[n] = '\0';
    return out;
}

int main(void)
{
    char *p = prefix("filename.txt", 8);
    if (!p) {
        fprintf(stderr, "m12-e06: out of memory\n");
        return EXIT_OOM;
    }
    check(strcmp(p, "filename") == 0, "prefix is the stem");
    free(p);
    return checks_done("m12-e06-off-by-one-nul");
}
