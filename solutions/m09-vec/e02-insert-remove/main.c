/* m09-e02-insert-remove: positional insert and remove. */
#define _DEFAULT_SOURCE            /* reallocarray(); must come before any #include */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"
#include "vec.h"

/* ---- Provided, already correct. ---- */

/* Makes room for one more element. False, *v unchanged, on failure. */
static bool vec_grow_one(VecInt *v)
{
    if (v->len < v->cap)
        return true;
    if (v->cap > SIZE_MAX / 2)
        return false;
    size_t new_cap = v->cap ? v->cap * 2 : 8;
    int *tmp = reallocarray(v->data, new_cap, sizeof *v->data);
    if (!tmp)
        return false;
    v->data = tmp;
    v->cap = new_cap;
    return true;
}

bool vec_push(VecInt *v, int x)
{
    if (!vec_grow_one(v))
        return false;
    v->data[v->len++] = x;
    return true;
}

void vec_destroy(VecInt *v)
{
    free(v->data);
    *v = (VecInt){0};
}

/* ---- Harness ---- */

static int same(const VecInt *v, const int *want, size_t n)
{
    return v->len == n && (n == 0 || memcmp(v->data, want, n * sizeof *want) == 0);
}

static int oom(VecInt *v)
{
    vec_destroy(v);
    fprintf(stderr, "m09-e02-insert-remove: out of memory\n");
    return EXIT_OOM;
}

int main(void)
{
    VecInt v = {0};
    int out = -1;
    check(!vec_remove(&v, 0, &out) && out == -1, "remove from empty fails");
    check(!vec_insert(&v, 1, 9) && v.len == 0, "insert past len fails");

    if (!vec_insert(&v, 0, 20)) return oom(&v);            /* [20] */
    if (!vec_insert(&v, 0, 10)) return oom(&v);            /* [10 20] */
    if (!vec_insert(&v, 2, 40)) return oom(&v);            /* [10 20 40] at len: append */
    if (!vec_insert(&v, 2, 30)) return oom(&v);            /* [10 20 30 40] */
    check(same(&v, (int[]){10, 20, 30, 40}, 4), "inserts at front, end and middle");

    for (int i = 0; i < 20; i++)                            /* force several growths mid-insert */
        if (!vec_insert(&v, 1, 100 + i)) return oom(&v);
    check(v.len == 24 && v.data[1] == 119 && v.data[20] == 100 && v.data[21] == 20, "inserts across growth");
    for (int i = 0; i < 20; i++)
        if (!vec_remove(&v, 1, NULL)) { check(0, "remove in range"); break; }
    check(same(&v, (int[]){10, 20, 30, 40}, 4), "removes undo inserts, out may be NULL");

    check(vec_remove(&v, 3, &out) && out == 40, "remove last");
    check(vec_remove(&v, 0, &out) && out == 10, "remove first");
    check(same(&v, (int[]){20, 30}, 2), "order kept");
    out = -1;
    check(!vec_remove(&v, 2, &out) && out == -1 && v.len == 2, "remove at len fails, nothing changes");
    check(vec_remove(&v, 1, &out) && vec_remove(&v, 0, &out) && v.len == 0 && out == 20, "remove to empty");

    vec_destroy(&v);
    return checks_done("m09-e02-insert-remove");
}

/* ---- Your definitions of vec_insert and vec_remove go below this line. ---- */

bool vec_insert(VecInt *v, size_t i, int x)
{
    if (i > v->len || !vec_grow_one(v))
        return false;
    memmove(&v->data[i + 1], &v->data[i], (v->len - i) * sizeof *v->data);
    v->data[i] = x;
    v->len++;
    return true;
}

bool vec_remove(VecInt *v, size_t i, int *out)
{
    if (i >= v->len)
        return false;
    if (out)
        *out = v->data[i];
    memmove(&v->data[i], &v->data[i + 1], (v->len - i - 1) * sizeof *v->data);
    v->len--;
    return true;
}
