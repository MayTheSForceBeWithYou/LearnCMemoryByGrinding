#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "scores.h"

int main(int argc, char **argv)
{
    const char *path = argc > 1 ? argv[1] : "scores.txt";
    struct scores *s = NULL;

    if (scores_load(path, &s) != 0) {
        fprintf(stderr, "m06-e03-goto-cleanup: could not load %s (out of memory?)\n", path);
        return EXIT_OOM;
    }
    check(strcmp(s->title, "Weekly high scores") == 0, "title without the newline");
    check(s->n == 8 && s->vals[0] == 120 && s->vals[4] == -4 && s->vals[7] == 42, "values");
    scores_destroy(s);

    struct scores *const SENTINEL = (struct scores *)&path;   /* any non-NULL marker */
    struct scores *t = SENTINEL;
    check(scores_load("no-such-file.txt", &t) == -1 && t == SENTINEL, "missing file: -1, *out untouched");
    t = SENTINEL;
    int rc = scores_load("bad.txt", &t);
    check(rc == -1 && t == SENTINEL, "malformed number: -1, *out untouched");
    scores_destroy(NULL);
    return checks_done("m06-e03-goto-cleanup");
}
