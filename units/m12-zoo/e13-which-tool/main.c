/* m12-e13-which-tool [PREDICT]: write PREDICTION.md before you run anything.
 * Each scenario contains one deliberate defect. Run one with: build/plain/prog <name> */
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* nul: a block of strlen(word) bytes receives word and its NUL. */
static int scenario_nul(const char *word)
{
    char *p = malloc(strlen(word));
    if (!p)
        return EXIT_OOM;
    strcpy(p, word);
    int c = p[0];
    free(p);
    return c == 'h' ? 0 : 1;
}

/* uninit: a decision depends on a heap int that was never written. */
static int scenario_uninit(void)
{
    int *v = malloc(4 * sizeof *v);
    if (!v)
        return EXIT_OOM;
    v[0] = v[1] = v[2] = 1;
    if (v[3] > 0)
        printf("positive\n");
    else
        printf("not positive\n");
    free(v);
    return 0;
}

/* stack: a write one element past a local array. */
static int scenario_stack(int n)
{
    int a[4];
    for (int i = 0; i <= n; i++)
        a[i] = i;
    return a[0];
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s nul|uninit|stack\n", argv[0]);
        return 2;
    }
    if (strcmp(argv[1], "nul") == 0)
        return scenario_nul("hello");
    if (strcmp(argv[1], "uninit") == 0)
        return scenario_uninit();
    if (strcmp(argv[1], "stack") == 0)
        return scenario_stack(argc + 2);
    return 2;
}
