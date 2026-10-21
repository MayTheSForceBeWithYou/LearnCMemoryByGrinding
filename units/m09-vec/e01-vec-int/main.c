/* m09-e01-vec-int: a growable array of int. */
#define _DEFAULT_SOURCE            /* reallocarray(); must come before any #include */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "course.h"
#include "vec.h"

enum { N = 200000 };

int main(void)
{
    VecInt v = {0};
    int x = -1;
    check(!vec_pop(&v, &x) && x == -1, "pop on empty fails, out untouched");
    check(!vec_get(&v, 0, &x) && x == -1, "get on empty fails");

    size_t grows = 0, prev_cap = 0;
    int geometric = 1;
    for (int i = 0; i < N; i++) {
        if (!vec_push(&v, i * 3)) {
            vec_destroy(&v);
            fprintf(stderr, "m09-e01-vec-int: out of memory\n");
            return EXIT_OOM;
        }
        if (v.cap != prev_cap) {
            if (prev_cap > 0 && v.cap < prev_cap + prev_cap / 4)
                geometric = 0;              /* this growth added less than a quarter */
            grows++;
            prev_cap = v.cap;
        }
    }
    check(v.len == N && v.cap >= v.len, "len and cap after pushes");
    check(geometric, "every growth is at least x1.25 (new cap >= old + old / 4)");
    check(grows < 64, "few growths in total");

    int all = 1;
    for (size_t i = 0; i < v.len; i++)
        all &= vec_get(&v, i, &x) && x == (int)i * 3;
    check(all, "every element readable in order");
    x = -1;
    check(!vec_get(&v, N, &x) && x == -1, "get at len fails");

    check(vec_pop(&v, &x) && x == (N - 1) * 3 && v.len == N - 1, "pop returns the last element");

    vec_destroy(&v);
    check(v.data == NULL && v.len == 0 && v.cap == 0, "destroy leaves an empty vector");
    vec_destroy(&v);

    /* An absurd capacity must make push fail cleanly instead of overflowing. */
    VecInt big = {NULL, SIZE_MAX / 2 + 1, SIZE_MAX / 2 + 1};
    check(!vec_push(&big, 1), "capacity overflow makes push fail");
    check(big.data == NULL && big.len == SIZE_MAX / 2 + 1, "failed push changed nothing");
    /* Doubling this count fits in size_t, but the byte count (cap * sizeof(int)) does not.
     * A crash here means the byte count (cap * sizeof) is not checked for overflow: the wrapped
     * realloc succeeded with a tiny block, and the push wrote far past its end. */
    VecInt bytes = {NULL, (size_t)1 << 61, (size_t)1 << 61};
    check(!vec_push(&bytes, 1), "byte-count overflow makes push fail");
    check(bytes.data == NULL && bytes.len == (size_t)1 << 61 && bytes.cap == (size_t)1 << 61, "failed push changed nothing");

    return checks_done("m09-e01-vec-int");
}

/* ---- Your definitions of the vec.h functions go below this line. ---- */
