/* M04 e04: predict realloc. Run one scenario with: make run ARGS=N  (or make valgrind ARGS=N, make asan ARGS=N)
 * With no argument, scenarios 1, 5 and 6 run with checks (this is what make check does).
 * Scenarios 2, 3 and 4 contain deliberate bugs. Predict first. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"

static void *volatile launder;
static void *hide(void *p) { launder = p; return launder; }

static int oom(void) { fprintf(stderr, "m04-e04-predict-realloc: out of memory\n"); return EXIT_OOM; }

static int s1(void)              /* realloc(NULL, 32) */
{
    char *p = realloc(NULL, 32);
    if (p == NULL)
        return oom();
    memset(p, 'a', 32);
    free(p);
    return 0;
}

static int s2(void)              /* grow 32 -> 4096, then read the grown tail */
{
    char *p = malloc(32);
    if (p == NULL)
        return oom();
    memset(p, 'a', 32);
    char *tmp = realloc(p, 4096);
    if (tmp == NULL) { free(p); return oom(); }
    p = tmp;
    int r = p[1000] == 'a' ? 1 : 0;   /* uninitialised read */
    printf("tail byte %s 'a'\n", r ? "==" : "!=");
    free(p);
    return 0;
}

static int s3(void)              /* shrink 4096 -> 16, read offsets 15 and 16 */
{
    char *p = malloc(4096);
    if (p == NULL)
        return oom();
    memset(p, 'b', 4096);
    char *tmp = realloc(p, 16);
    if (tmp == NULL) { free(p); return oom(); }
    p = tmp;
    printf("offset 15: %c\n", p[15]);
    printf("offset 16: %c\n", ((char *)hide(p))[16]);   /* one past the new end */
    free(p);
    return 0;
}

static int s4(void)              /* realloc(p, 0), then free the original */
{
    char *p = malloc(64);
    if (p == NULL)
        return oom();
    size_t zero = (size_t)(uintptr_t)hide(NULL);   /* 0, hidden from the compiler */
    char *q = realloc(p, zero);
    printf("realloc(p, 0) returned %s\n", q ? "non-NULL" : "NULL");
    free(hide(p));
    free(q);
    return 0;
}

/* Grows a buffer to 10001 bytes. After each grow, one small "other" allocation is made,
 * as any real program would, so the buffer cannot always just extend into free space. */
static int count_moves(int doubling, int *moves)
{
    static void *others[10000];
    size_t nothers = 0;
    size_t cap = 1;
    int rc = 0;
    char *p = malloc(cap);
    if (p == NULL)
        return oom();
    *moves = 0;
    for (int i = 0; i < 10000; i++) {
        size_t need = (size_t)i + 2;
        if (need <= cap)
            continue;
        size_t newcap = doubling ? cap * 2 : cap + 1;
        uintptr_t before = (uintptr_t)p;
        char *tmp = realloc(p, newcap);
        if (tmp == NULL) { rc = oom(); break; }
        *moves += (uintptr_t)tmp != before;
        p = tmp;
        cap = newcap;
        if ((others[nothers] = malloc(16)) == NULL) { rc = oom(); break; }
        nothers++;
    }
    for (size_t k = 0; k < nothers; k++)
        free(others[k]);
    free(p);
    return rc;
}

static int s5(void)
{
    int moves;
    int rc = count_moves(0, &moves);
    if (rc == 0)
        printf("grow by 1, 10000 times: %d moves\n", moves);
    return rc;
}

static int s6(void)
{
    int moves;
    int rc = count_moves(1, &moves);
    if (rc == 0)
        printf("doubling to 10000: %d moves\n", moves);
    return rc;
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
        return scen[k - 1]();
    }
    check(s1() == 0, "s1");
    check(s5() == 0, "s5");
    check(s6() == 0, "s6");
    return checks_done("m04-e04-predict-realloc");
}
