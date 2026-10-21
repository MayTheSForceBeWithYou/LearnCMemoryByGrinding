/* m08-e01-split-words: split a line into an owned, NULL-terminated table of words. */
#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"

/* Returns a NULL-terminated table of heap copies of the words in line.
 * *count_out (if count_out != NULL) receives the word count.
 * Caller owns the table and every string; release with free_words().
 * No words: a table holding only NULL. Allocation failure: NULL, nothing leaked. */
char **split_words(const char *line, size_t *count_out);

/* Frees every word, then the table. Accepts NULL. */
void free_words(char **words);

static int expect(const char *line, const char *const *want, size_t n)
{
    size_t count = 12345;
    char **w = split_words(line, &count);
    if (!w)
        return 0;
    check(count == n, "word count");
    for (size_t i = 0; i < n && i < count; i++)
        check(w[i] && strcmp(w[i], want[i]) == 0, "word text");
    check(count != n || w[n] == NULL, "table is NULL-terminated");
    free_words(w);
    return 1;
}

int main(void)
{
    static const char *const a[] = {"the", "quick", "brown", "fox"};
    static const char *const b[] = {"x"};
    static const char *const c[] = {"tab\x01", "\xc3\xa9t\xc3\xa9", "end"};
    int ok = expect("the quick brown fox", a, 4)
          && expect("   the  quick\tbrown\n fox  ", a, 4)
          && expect("x", b, 1)
          && expect("", NULL, 0)
          && expect(" \t\n ", NULL, 0)
          && expect("tab\x01 \xc3\xa9t\xc3\xa9\vend\r", c, 3);
    if (ok) {
        char **w = split_words("no count wanted", NULL);
        if (!w)
            ok = 0;
        else {
            check(w[0] && strcmp(w[2], "wanted") == 0 && w[3] == NULL, "count_out may be NULL");
            free_words(w);
        }
    }
    free_words(NULL);
    if (!ok) {
        fprintf(stderr, "m08-e01-split-words: out of memory\n");
        return EXIT_OOM;
    }
    return checks_done("m08-e01-split-words");
}

/* ---- Your definitions of split_words and free_words go below this line. ---- */
