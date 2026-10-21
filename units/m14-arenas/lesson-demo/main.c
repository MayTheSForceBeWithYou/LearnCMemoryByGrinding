/* M14 lesson demo (not an exercise): alignment facts on this machine. */
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "course.h"

int main(void)
{
    printf("alignof(char)        = %zu\n", alignof(char));
    printf("alignof(int)         = %zu\n", alignof(int));
    printf("alignof(double)      = %zu\n", alignof(double));
    printf("alignof(long double) = %zu\n", alignof(long double));
    printf("alignof(max_align_t) = %zu\n", alignof(max_align_t));
    printf("sizeof(max_align_t)  = %zu\n", sizeof(max_align_t));
    void *p = malloc(1), *q = malloc(1);
    if (!p || !q)
        return EXIT_OOM;
    printf("malloc(1) %% 16       = %zu, %zu\n", (size_t)((uintptr_t)p % 16), (size_t)((uintptr_t)q % 16));
    printf("distance between them = %td bytes\n", (char *)q - (char *)p);
    free(p);
    free(q);
    return 0;
}
