/* m14-e03-pool-freelist [WRITE]: reference solution. */
#include <stdlib.h>
#include <stdalign.h>
#include <stdint.h>
#include "course.h"
#include "pool.h"


/* A free slot's first bytes hold the pointer to the next free slot. The
 * slot memory came from malloc and has no declared type, so storing a
 * void * there gives it that effective type until the slot is reused. */

int pool_init(Pool *p, size_t objsize, size_t count)
{
    const size_t align = alignof(max_align_t);
    p->block = NULL;
    p->free_list = NULL;
    p->slot = p->count = 0;
    if (objsize == 0 || count == 0)
        return -1;
    size_t size = objsize < sizeof(void *) ? sizeof(void *) : objsize;
    if (size > SIZE_MAX - (align - 1))
        return -1;
    size = (size + align - 1) & ~(align - 1);
    if (count > SIZE_MAX / size)
        return -1;
    p->block = malloc(count * size);
    if (!p->block)
        return -1;
    p->slot = size;
    p->count = count;
    for (size_t i = count; i > 0; i--) {
        void *s = p->block + (i - 1) * size;
        *(void **)s = p->free_list;
        p->free_list = s;
    }
    return 0;
}

void *pool_get(Pool *p)
{
    void *s = p->free_list;
    if (s)
        p->free_list = *(void **)s;
    return s;
}

void pool_put(Pool *p, void *obj)
{
    if (!obj)
        return;
    *(void **)obj = p->free_list;
    p->free_list = obj;
}

void pool_destroy(Pool *p)
{
    free(p->block);
    p->block = NULL;
    p->free_list = NULL;
    p->slot = p->count = 0;
}

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
