/* m09-e03-growth-count: two growth policies, same pushes. Predict FA_COUNT first. */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "course.h"

enum { COUNT = 1000, FIRST_CAP = 8 };

typedef struct {
    int    *data;
    size_t  len, cap;
} VecInt;

/* Doubling: start at FIRST_CAP, double whenever full. */
static bool push_doubling(VecInt *v, int x)
{
    if (v->len == v->cap) {
        if (v->cap > SIZE_MAX / 2 / sizeof *v->data)
            return false;                   /* new_cap * sizeof would overflow */
        size_t new_cap = v->cap ? v->cap * 2 : FIRST_CAP;
        int *tmp = realloc(v->data, new_cap * sizeof *tmp);
        if (!tmp)
            return false;
        v->data = tmp;
        v->cap = new_cap;
    }
    v->data[v->len++] = x;
    return true;
}

/* Grow-by-one: exactly enough room for the new element, every time. */
static bool push_by_one(VecInt *v, int x)
{
    if (v->len == v->cap) {
        if (v->cap >= SIZE_MAX / sizeof *v->data)
            return false;                   /* (cap + 1) * sizeof would overflow */
        int *tmp = realloc(v->data, (v->cap + 1) * sizeof *tmp);
        if (!tmp)
            return false;
        v->data = tmp;
        v->cap++;
    }
    v->data[v->len++] = x;
    return true;
}

int main(void)
{
    VecInt a = {0}, b = {0};
    int ok = 1;
    for (int i = 0; i < COUNT && ok; i++)
        ok = push_doubling(&a, i) && push_by_one(&b, i);
    if (!ok) {
        free(a.data);
        free(b.data);
        fprintf(stderr, "m09-e03-growth-count: out of memory\n");
        return EXIT_OOM;
    }
    printf("doubling:    len %zu  cap %zu\n", a.len, a.cap);
    printf("grow-by-one: len %zu  cap %zu\n", b.len, b.cap);
    check(a.len == COUNT && b.len == COUNT, "both hold every element");
    check(a.data[COUNT - 1] == COUNT - 1 && b.data[COUNT - 1] == COUNT - 1, "last element");
    free(a.data);
    free(b.data);
    return checks_done("m09-e03-growth-count");
}
