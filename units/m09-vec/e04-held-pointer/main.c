/* m09-e04-held-pointer: keep the best score seen so far while filling a vector. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "course.h"

typedef struct {
    int    *data;
    size_t  len, cap;
} VecInt;

static bool vec_push(VecInt *v, int x)
{
    if (v->len == v->cap) {
        size_t new_cap = v->cap ? v->cap * 2 : 4;
        int *tmp = realloc(v->data, new_cap * sizeof *tmp);
        if (!tmp)
            return false;
        v->data = tmp;
        v->cap = new_cap;
    }
    v->data[v->len++] = x;
    return true;
}

int main(void)
{
    static const int scores[] = {12, 40, 7, 33, 51, 9, 18, 44, 2, 30};
    const size_t n = sizeof scores / sizeof scores[0];
    VecInt v = {0};
    int *best = NULL;   /* points at the highest score stored so far */

    for (size_t i = 0; i < n; i++) {
        if (!vec_push(&v, scores[i])) {
            free(v.data);
            fprintf(stderr, "m09-e04-held-pointer: out of memory\n");
            return EXIT_OOM;
        }
        if (best == NULL || scores[i] > *best)
            best = &v.data[v.len - 1];
    }
    printf("best score: %d\n", *best);
    check(*best == 51, "best is 51");
    free(v.data);
    return checks_done("m09-e04-held-pointer");
}
