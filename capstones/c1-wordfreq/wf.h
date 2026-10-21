#ifndef WF_H
#define WF_H

#include <stddef.h>
#include <stdio.h>

/* A word-frequency table: a growable hash table that OWNS a copy of every word. */
struct wf;

struct wf_entry {
    const char *word;   /* borrowed from the table: valid until the next wf_add or wf_free */
    size_t count;
};

/* New empty table; free with wf_free. NULL on allocation failure. */
struct wf *wf_new(void);
/* Free the table and every word it owns. wf_free(NULL) does nothing. */
void wf_free(struct wf *t);
/* Count one occurrence of word[0..len-1] (need not be NUL-terminated; copied).
 * Returns 0 on success, -1 on allocation failure; on failure the table is
 * unchanged and still valid. */
int wf_add(struct wf *t, const char *word, size_t len);
/* Number of distinct words. */
size_t wf_size(const struct wf *t);
/* Count for word (NUL-terminated), 0 if absent. */
size_t wf_count(const struct wf *t, const char *word);
/* A new array of all entries sorted by count descending, then word ascending
 * (strcmp). *n receives the length. Caller frees the array (only the array:
 * the words are borrowed). NULL on allocation failure; an empty table gives a
 * non-NULL array with *n == 0. */
struct wf_entry *wf_sorted(const struct wf *t, size_t *n);
/* Read words from f into t: a word is a maximal run of ASCII letters and
 * apostrophes, lower-cased. Returns 0 on success; -1 on allocation failure;
 * -2 on a read error (ferror(f) set). On failure, words read so far stay
 * counted and the table is still valid. */
int wf_read(struct wf *t, FILE *f);

#endif
