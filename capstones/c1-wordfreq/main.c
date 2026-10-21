#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "wf.h"

#define OOM(what) do { fprintf(stderr, "c1-wordfreq: out of memory (%s)\n", what); goto oom; } while (0)

static int unit_checks(void)
{
    struct wf *t = wf_new();
    if (t == NULL)
        return -1;
    int rc = -1;
    if (wf_add(t, "applesauce", 5) != 0 || wf_add(t, "apple", 5) != 0 || wf_add(t, "pear", 4) != 0)
        goto out;
    check(wf_count(t, "apple") == 2, "len-bounded add copies only len bytes");
    check(wf_count(t, "applesauce") == 0, "longer word absent");
    check(wf_size(t) == 2, "two distinct words");

    char key[16];
    for (int i = 0; i < 2000; i++) {             /* forces several rehashes */
        snprintf(key, sizeof key, "w%d", i % 500);
        if (wf_add(t, key, strlen(key)) != 0)
            goto out;
    }
    check(wf_size(t) == 502, "502 distinct after growth");
    check(wf_count(t, "w499") == 4, "counts survive rehash");
    rc = 0;
out:
    wf_free(t);
    return rc;
}

int main(int argc, char **argv)
{
    struct wf *t = NULL;
    struct wf_entry *e = NULL;
    FILE *f = NULL;

    if (argc != 2) {
        fprintf(stderr, "usage: %s FILE\n", argv[0]);
        return 2;
    }
    if (unit_checks() != 0)
        OOM("unit checks");

    f = fopen(argv[1], "r");
    if (f == NULL) {
        perror(argv[1]);
        return 2;
    }
    t = wf_new();
    if (t == NULL)
        OOM("table");
    int rr = wf_read(t, f);
    if (rr == -2) {
        fprintf(stderr, "c1-wordfreq: read error on %s\n", argv[1]);
        fclose(f);
        wf_free(t);
        return 2;
    }
    if (rr != 0)
        OOM("reading");
    fclose(f);
    f = NULL;

    size_t n;
    e = wf_sorted(t, &n);
    if (e == NULL)
        OOM("sorting");
    for (size_t i = 0; i < n && i < 5; i++)
        printf("%7zu %s\n", e[i].count, e[i].word);

    check(wf_count(t, "the") == 7, "sample: 'the' x7");
    check(wf_count(t, "don't") == 2, "sample: apostrophes kept");
    check(wf_count(t, "The") == 0, "lookups are exact; input was lower-cased");
    check(n == wf_size(t), "sorted length matches size");
    check(n > 0 && strcmp(e[0].word, "the") == 0, "most frequent first");
    int ordered = 1;
    for (size_t i = 1; i < n; i++)
        if (e[i - 1].count < e[i].count ||
            (e[i - 1].count == e[i].count && strcmp(e[i - 1].word, e[i].word) >= 0))
            ordered = 0;
    check(ordered, "count desc, then word asc");

    free(e);
    wf_free(t);
    return checks_done("c1-wordfreq");

oom:
    if (f != NULL)
        fclose(f);
    free(e);
    wf_free(t);
    return EXIT_OOM;
}
