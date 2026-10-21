/* M02 e06: trace the blocks. Complete program; draw p, q, r and the blocks at TRACE 1..5.
 * Addresses are recorded as uintptr_t before any free, so nothing here touches a
 * dangling pointer. The program prints which variables alias which. */
#include <stdint.h>
#include <stdlib.h>
#include "course.h"

/* Behind-the-scenes backup so the course's leak checkers stay clean. Ignore it in your drawing. */
static void *backup;

static void show(int mark, uintptr_t p, uintptr_t q, uintptr_t r, uintptr_t b1, uintptr_t b2)
{
    printf("TRACE %d: p=%s q=%s r=%s\n", mark,
           p == 0 ? "NULL" : p == b1 ? "B1-addr" : p == b2 ? "B2-addr" : "?",
           q == 0 ? "NULL" : q == b1 ? "B1-addr" : q == b2 ? "B2-addr" : "?",
           r == 0 ? "NULL" : r == b1 ? "B1-addr" : r == b2 ? "B2-addr" : "?");
}

int main(void)
{
    char *p = malloc(24);
    char *q = NULL;
    char *r = NULL;
    if (p == NULL) {
        fprintf(stderr, "m02-e06-trace-free: out of memory\n");
        return EXIT_OOM;
    }
    uintptr_t b1 = (uintptr_t)p, b2 = 0;
    q = p;
    /* TRACE 1 */
    show(1, (uintptr_t)p, (uintptr_t)q, (uintptr_t)r, b1, b2);

    r = malloc(16);
    if (r == NULL) {
        free(p);
        fprintf(stderr, "m02-e06-trace-free: out of memory\n");
        return EXIT_OOM;
    }
    b2 = (uintptr_t)r;
    uintptr_t qv = (uintptr_t)q;
    free(p);
    p = NULL;
    /* TRACE 2 */
    show(2, (uintptr_t)p, qv, (uintptr_t)r, b1, b2);

    q = r;
    /* TRACE 3 */
    show(3, (uintptr_t)p, (uintptr_t)q, (uintptr_t)r, b1, b2);

    backup = r;
    r = NULL;
    /* TRACE 4 */
    show(4, (uintptr_t)p, (uintptr_t)q, (uintptr_t)r, b1, b2);

    q = NULL;
    /* TRACE 5 */
    show(5, (uintptr_t)p, (uintptr_t)q, (uintptr_t)r, b1, b2);

    free(backup);
    check(p == NULL && q == NULL && r == NULL, "all three end NULL");
    return checks_done("m02-e06-trace-free");
}
