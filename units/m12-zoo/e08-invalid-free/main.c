/* m12-e08-invalid-free [BUG] */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* Counts the words in a line: runs of non-space characters. */
static int count_words(const char *line, size_t *out)
{
    char *s = strdup(line);
    if (!s)
        return -1;
    size_t words = 0;
    while (*s) {
        while (*s == ' ')
            s++;
        if (*s)
            words++;
        while (*s && *s != ' ')
            s++;
    }
    free(s);
    *out = words;
    return 0;
}

int main(void)
{
    size_t n = 0;
    if (count_words("  the quick  brown fox ", &n) != 0) {
        fprintf(stderr, "m12-e08: out of memory\n");
        return EXIT_OOM;
    }
    check(n == 4, "four words");
    return checks_done("m12-e08-invalid-free");
}
