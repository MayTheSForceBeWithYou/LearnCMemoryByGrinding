/* m14-e01-bump-arena [WRITE] */
#include <stdalign.h>
#include <stdint.h>
#include "arena.h"
#include "course.h"

/* Write arena_init, arena_alloc, arena_reset and arena_free (see arena.h). */

static int aligned(const void *p)
{
    return (uintptr_t)p % alignof(max_align_t) == 0;
}

int main(void)
{
    Arena a;
    if (arena_init(&a, 256) != 0) {
        arena_free(&a);
        fprintf(stderr, "m14-e01: out of memory\n");
        return EXIT_OOM;
    }
    char *s = arena_alloc(&a, 5);
    double *d = arena_alloc(&a, sizeof *d);
    long double *ld = arena_alloc(&a, sizeof *ld);
    check(s && d && ld, "three small requests fit");
    check(aligned(s) && aligned(d) && aligned(ld), "every pointer is max_align_t-aligned");
    if (d && ld) {
        *d = 2.5;
        *ld = 1.25L;
        check(*d == 2.5 && *ld == 1.25L, "objects do not overlap");
    }
    check(arena_alloc(&a, 0) == NULL, "size 0 gives NULL");
    check(arena_alloc(&a, SIZE_MAX) == NULL, "SIZE_MAX gives NULL without wrapping");
    check(arena_alloc(&a, SIZE_MAX - 8) == NULL, "rounding SIZE_MAX - 8 must not wrap");
    check(arena_alloc(&a, 512) == NULL, "too large gives NULL");
    check(arena_alloc(&a, a.cap - a.used) != NULL, "the exact remainder fits");
    check(arena_alloc(&a, 1) == NULL, "a full arena gives NULL");
    arena_reset(&a);
    check(arena_alloc(&a, 1) == (void *)a.base, "after reset, allocation starts at the base");
    arena_free(&a);
    check(a.base == NULL && a.cap == 0 && a.used == 0, "arena_free empties the arena");
    return checks_done("m14-e01-bump-arena");
}
