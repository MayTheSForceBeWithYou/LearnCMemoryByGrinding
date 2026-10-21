/* m13-e01-break-on-alloc [TRACE]: fill in the table in README.md using gdb. */
#include <stdlib.h>
#include "course.h"

typedef struct {
    int *data; /* owned, room for cap ints */
    size_t len, cap;
} Vec;

/* Appends x. Returns 0, or -1 on allocation failure with v unchanged. */
static int vec_push(Vec *v, int x)
{
    if (v->len == v->cap) {
        size_t ncap = v->cap ? v->cap * 2 : 1;
        int *p = realloc(v->data, ncap * sizeof *p);
        if (!p)
            return -1;
        v->data = p;
        v->cap = ncap;
    }
    v->data[v->len++] = x;
    return 0;
}

int main(void)
{
    Vec v = {NULL, 0, 0};
    for (int i = 0; i < 10; i++) {
        if (vec_push(&v, i * i) != 0) {
            free(v.data);
            fprintf(stderr, "m13-e01: out of memory\n");
            return EXIT_OOM;
        }
    }
    /* POINT B */
    check(v.len == 10 && v.cap == 16, "ten pushes, capacity 16");
    check(v.data[9] == 81, "last element is 81");
    free(v.data);
    return checks_done("m13-e01-break-on-alloc");
}
