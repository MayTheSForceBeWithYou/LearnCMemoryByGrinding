/* M02 e04: what does this free do?  Run one scenario: build/plain/prog N  (or make run ARGS=N)
 * With no argument, only the defined scenarios 1 and 2 run (this is what make check does).
 * Scenarios 3-6 are deliberately undefined behaviour. Predict first, then run. */
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* Keeps the compiler from seeing through our bad frees and warning or optimising. */
static void *volatile launder;
static void *hide(void *p) { launder = p; return launder; }

static int s1(void)                 /* free(NULL) */
{
    free(NULL);
    return 0;
}

static int s2(void)                 /* free, then NULL the pointer, then free again */
{
    int *p = malloc(4 * sizeof *p);
    if (p == NULL)
        return EXIT_OOM;
    free(p);
    p = NULL;
    free(p);
    return 0;
}

static int s3(void)                 /* double free, nothing in between */
{
    int *p = malloc(4 * sizeof *p);
    if (p == NULL)
        return EXIT_OOM;
    free(p);
    free(hide(p));
    return 0;
}

static int s4(void)                 /* double free with another allocation in between */
{
    int *p = malloc(4 * sizeof *p);
    if (p == NULL)
        return EXIT_OOM;
    free(p);
    int *q = malloc(4 * sizeof *q);  /* may receive the same chunk */
    if (q == NULL)
        return EXIT_OOM;
    free(hide(p));
    free(q);
    return 0;
}

static int s5(void)                 /* free a stack array */
{
    int local[4] = {1, 2, 3, 4};
    free(hide(local));
    return local[0] == 1 ? 0 : 1;
}

static int s6(void)                 /* free an interior pointer */
{
    int *p = malloc(4 * sizeof *p);
    if (p == NULL)
        return EXIT_OOM;
    free(hide(p + 1));
    return 0;
}

int main(int argc, char **argv)
{
    int (*const scen[])(void) = { s1, s2, s3, s4, s5, s6 };

    if (argc > 1) {
        int k = atoi(argv[1]);
        if (!(k >= 1 && k <= 6)) {
            fprintf(stderr, "usage: %s N   (N >= 1 && N <= 6)\n", argv[0]);
            return 2;
        }
        int rc = scen[k - 1]();
        printf("scenario %d returned %d\n", k, rc);
        return rc;
    }
    check(s1() == 0, "s1: free(NULL) is a no-op");
    check(s2() == 0, "s2: free after p = NULL is free(NULL)");
    return checks_done("m02-e04-predict-free");
}
