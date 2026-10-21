/* M01 e01: trace the frames. Complete program; draw memory at points A, B, C. */
#include <stdio.h>
#include <stdlib.h>
#include "course.h"

static int *inner(int *acc)
{
    static int calls = 0;
    int local = *acc * 2;

    calls++;
    *acc = local + calls;
    /* TRACE POINT A */
    printf("A: calls=%d local=%d *acc=%d\n", calls, local, *acc);
    return acc;
}

static int outer(int n)
{
    int total = n;
    int *heap = malloc(sizeof *heap);

    if (heap == NULL)
        return -1;
    *heap = n + 100;
    int *r = inner(&total);
    /* TRACE POINT B */
    printf("B: total=%d *r=%d *heap=%d\n", total, *r, *heap);
    int result = total + *heap;
    free(heap);
    return result;
}

int main(void)
{
    int first = outer(3);
    int second = outer(3);

    if (first < 0 || second < 0) {
        fprintf(stderr, "out of memory\n");
        return EXIT_OOM;
    }
    /* TRACE POINT C */
    printf("C: first=%d second=%d\n", first, second);
    check(first == 110, "first call: total 3 -> 7, heap 103");
    check(second == 111, "second call sees calls==2: total 3 -> 8");
    return checks_done("m01-e01-trace-frames");
}
