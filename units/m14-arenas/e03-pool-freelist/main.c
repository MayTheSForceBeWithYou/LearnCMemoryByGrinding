/* m14-e03-pool-freelist [WRITE] */
#include <stdalign.h>
#include <stdint.h>
#include "course.h"
#include "pool.h"

/* Write pool_init, pool_get, pool_put and pool_destroy (see pool.h). */

typedef struct {
    double x, y;
    char name[9];
} Particle;

int main(void)
{
    Pool p;
    check(pool_init(&p, 0, 4) == -1, "objsize 0 is rejected");
    check(pool_init(&p, 8, SIZE_MAX / 4) == -1, "an overflowing size is rejected");
    if (pool_init(&p, sizeof(Particle), 3) != 0) {
        fprintf(stderr, "m14-e03: out of memory\n");
        return EXIT_OOM;
    }
    check(p.slot >= sizeof(Particle) && p.slot % alignof(max_align_t) == 0, "slot size");
    Particle *a = pool_get(&p), *b = pool_get(&p), *c = pool_get(&p);
    check(a && b && c && a != b && b != c && a != c, "three distinct slots");
    check(pool_get(&p) == NULL, "the pool is exhausted");
    if (a && b && c) {
        check((uintptr_t)b % alignof(max_align_t) == 0, "slots are aligned");
        a->x = 1.0;
        c->x = 3.0;
        b->x = 2.0;
        check(a->x == 1.0 && c->x == 3.0, "slots do not overlap");
        pool_put(&p, b);
        pool_put(&p, NULL);
        check(pool_get(&p) == b, "a returned slot is reused first");
        pool_put(&p, a);
        pool_put(&p, c);
        check(pool_get(&p) == c && pool_get(&p) == a, "free list is LIFO");
    }
    pool_destroy(&p);

    Pool tiny;
    if (pool_init(&tiny, 1, 2) != 0) {
        fprintf(stderr, "m14-e03: out of memory\n");
        return EXIT_OOM;
    }
    check(tiny.slot >= sizeof(void *), "a 1-byte object still gets a pointer-sized slot");
    pool_destroy(&tiny);
    return checks_done("m14-e03-pool-freelist");
}
