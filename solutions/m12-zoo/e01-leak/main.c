/* m12-e01-leak [BUG] */
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* Returns a lowercase heap copy of s without leading or trailing spaces.
 * The caller frees it. Returns NULL on allocation failure. */
static char *normalize(const char *s)
{
    while (*s == ' ')
        s++;
    size_t n = strlen(s);
    while (n > 0 && s[n - 1] == ' ')
        n--;
    char *out = malloc(n + 1);
    if (!out)
        return NULL;
    for (size_t i = 0; i < n; i++)
        out[i] = (char)tolower((unsigned char)s[i]);
    out[n] = '\0';
    return out;
}

/* Counts the words equal to target after normalization.
 * Returns SIZE_MAX on allocation failure. */
static size_t count_equal(const char *const *words, size_t n, const char *target)
{
    char *t = normalize(target);
    if (!t)
        return SIZE_MAX;
    size_t hits = 0;
    for (size_t i = 0; i < n; i++) {
        char *w = normalize(words[i]);
        if (!w) {
            free(t);
            return SIZE_MAX;
        }
        if (strcmp(w, t) == 0)
            hits++;
        free(w);
    }
    free(t);
    return hits;
}

int main(void)
{
    const char *words[] = {"Apple", " pear", "APPLE ", "plum", "apple"};
    size_t hits = count_equal(words, 5, "apple");
    if (hits == SIZE_MAX) {
        fprintf(stderr, "m12-e01: out of memory\n");
        return EXIT_OOM;
    }
    check(hits == 3, "three spellings of apple");
    check(count_equal(words, 5, "kiwi") == 0, "no kiwi");
    return checks_done("m12-e01-leak");
}
