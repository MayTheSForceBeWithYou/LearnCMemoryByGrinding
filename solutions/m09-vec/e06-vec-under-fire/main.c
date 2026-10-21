/* m09-e06-vec-under-fire: growth and shrinking that survive every allocation failure. */
#define _DEFAULT_SOURCE            /* reallocarray(); must come before any #include */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "course.h"

typedef struct {
    int    *data;
    size_t  len, cap;
} VecInt;

/* Ensures cap >= len + extra. False, and *v unchanged, on overflow or allocation failure. */
bool vec_reserve(VecInt *v, size_t extra);
/* Appends x. False, and *v unchanged, on failure. */
bool vec_push(VecInt *v, int x);
/* Reduces cap to len. Never fails visibly: if shrinking fails, *v keeps its buffer.
 * len == 0 frees the buffer and leaves {NULL, 0, 0}. Never calls realloc(p, 0). */
void vec_shrink_to_fit(VecInt *v);

/* ---- Provided, already correct. ---- */

static void vec_destroy(VecInt *v)
{
    free(v->data);
    *v = (VecInt){0};
}

static int intact(const VecInt *v, size_t n)
{
    if (v->len != n || v->cap < v->len)
        return 0;
    for (size_t i = 0; i < n; i++)
        if (v->data[i] != (int)(i * 7))
            return 0;
    return 1;
}

static int oom(VecInt *v, size_t expect_len)
{
    int ok = intact(v, expect_len);
    vec_destroy(v);
    if (!ok) {
        check(0, "a failed call left the vector intact");
        return checks_done("m09-e06-vec-under-fire");
    }
    fprintf(stderr, "m09-e06-vec-under-fire: out of memory\n");
    return EXIT_OOM;
}

int main(void)
{
    VecInt v = {0};
    VecInt huge = {NULL, 10, 10};
    check(!vec_reserve(&huge, SIZE_MAX - 5) && huge.cap == 10, "reserve overflow fails cleanly");

    for (size_t i = 0; i < 40; i++)
        if (!vec_push(&v, (int)(i * 7)))
            return oom(&v, i);
    if (!vec_reserve(&v, 200))
        return oom(&v, 40);
    check(v.cap >= 240, "reserve made room");
    int *before = v.data;
    for (size_t i = 40; i < 240; i++)
        if (!vec_push(&v, (int)(i * 7)))
            return oom(&v, i);
    check(v.data == before, "pushes within reserved room do not reallocate");

    v.len = 30;                                  /* drop elements; values are plain ints */
    vec_shrink_to_fit(&v);
    check(intact(&v, 30), "shrink keeps the elements (whether or not it shrank)");
    for (size_t i = 30; i < 60; i++)
        if (!vec_push(&v, (int)(i * 7)))
            return oom(&v, i);
    check(intact(&v, 60), "vector usable after shrink");

    v.len = 0;
    vec_shrink_to_fit(&v);
    check(v.data == NULL && v.cap == 0, "shrinking an empty vector frees the buffer");
    vec_destroy(&v);
    return checks_done("m09-e06-vec-under-fire");
}

/* ---- Your definitions of vec_reserve, vec_push and vec_shrink_to_fit go below this line. ---- */

static bool vec_set_cap(VecInt *v, size_t new_cap)
{
    int *tmp = reallocarray(v->data, new_cap, sizeof *v->data);
    if (!tmp)
        return false;                       /* old block untouched and still ours */
    v->data = tmp;
    v->cap = new_cap;
    return true;
}

bool vec_reserve(VecInt *v, size_t extra)
{
    if (extra > SIZE_MAX - v->len)
        return false;
    size_t need = v->len + extra;
    if (need <= v->cap)
        return true;
    size_t new_cap = v->cap ? v->cap : 8;
    while (new_cap < need)
        new_cap = new_cap > SIZE_MAX / 2 ? need : new_cap * 2;
    return vec_set_cap(v, new_cap);
}

bool vec_push(VecInt *v, int x)
{
    if (!vec_reserve(v, 1))
        return false;
    v->data[v->len++] = x;
    return true;
}

void vec_shrink_to_fit(VecInt *v)
{
    if (v->len == v->cap)
        return;
    if (v->len == 0) {
        free(v->data);
        *v = (VecInt){0};
        return;
    }
    (void)vec_set_cap(v, v->len);           /* failure: keep the bigger buffer */
}
