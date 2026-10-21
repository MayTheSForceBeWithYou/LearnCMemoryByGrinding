/* m09 lesson demo: does realloc move a block? Grow 64 -> 128 bytes, then shrink to 32,
 * 100 times, and count how often the address changed. Quoted in ../LESSON.md. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum { ROUNDS = 100 };

int main(void)
{
    int grow_moved = 0, shrink_moved = 0;
    for (int i = 0; i < ROUNDS; i++) {
        char *p = malloc(64);
        if (!p)
            return 3;
        uintptr_t before = (uintptr_t)p;
        char *q = realloc(p, 128);
        if (!q) {
            free(p);
            return 3;
        }
        grow_moved += (uintptr_t)q != before;
        before = (uintptr_t)q;
        char *r = realloc(q, 32);
        if (!r) {
            free(q);
            return 3;
        }
        shrink_moved += (uintptr_t)r != before;
        free(r);
    }
    printf("grow 64 -> 128:  moved %d of %d\n", grow_moved, ROUNDS);
    printf("shrink 128 -> 32: moved %d of %d\n", shrink_moved, ROUNDS);
    return 0;
}
