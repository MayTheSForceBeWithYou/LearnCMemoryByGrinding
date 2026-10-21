#include <stdint.h>
#include <stdlib.h>
#include "ints.h"

int ints_push(struct ints *v, int x)
{
    if (v->len == v->cap) {
        size_t newcap = v->cap ? v->cap : 8;
        if (v->cap) {
            if (newcap > SIZE_MAX / 2 / sizeof *v->data)
                return -1;
            newcap *= 2;
        }
        int *tmp = realloc(v->data, newcap * sizeof *tmp);   /* realloc(NULL, n) on the first push */
        if (tmp == NULL)
            return -1;                                      /* *v untouched */
        v->data = tmp;
        v->cap = newcap;
    }
    v->data[v->len++] = x;
    return 0;
}

void ints_free(struct ints *v)
{
    free(v->data);
    v->data = NULL;
    v->len = v->cap = 0;
}
