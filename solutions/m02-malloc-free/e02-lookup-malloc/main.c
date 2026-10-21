/* M02 e02: probe what man 3 malloc describes. Complete program.
 *
 *   make run                 the defined observations (this is what make check runs)
 *   make run ARGS=peek       also (a) asks for SIZE_MAX bytes and prints errno, and
 *                            (b) prints bytes of a recycled malloc block. (b) reads
 *                            indeterminate memory: it is here only so you can see
 *                            when MALLOC_PERTURB_ does and doesn't change it. Never
 *                            do this in real code.
 *   MALLOC_PERTURB_=165 make run ARGS=peek
 *   GLIBC_TUNABLES=glibc.malloc.tcache_count=0 MALLOC_PERTURB_=165 make run ARGS=peek
 *
 * peek is plain-build only: Valgrind would flag (b), and ASan's allocator doesn't
 * honour MALLOC_PERTURB_.
 */
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"

int main(int argc, char **argv)
{
    /* Q1: malloc(0) */
    void *z = malloc(0);
    printf("malloc(0) returned %s\n", z ? "a non-NULL pointer" : "NULL");
    free(z);

    /* Q4: free(NULL) */
    free(NULL);
    printf("free(NULL) returned normally\n");

    /* Q5: MALLOC_PERTURB_ */
    const char *perturb = getenv("MALLOC_PERTURB_");
    printf("MALLOC_PERTURB_ is %s\n", perturb ? perturb : "(unset)");
    if (argc > 1 && strcmp(argv[1], "peek") == 0) {
        /* Q2: errno on failure. SIZE_MAX bytes can never be satisfied.
         * volatile stops gcc from warning that the size is impossible. */
        volatile size_t too_big = SIZE_MAX;
        errno = 0;
        void *huge = malloc(too_big);
        int saved = errno;
        printf("malloc(SIZE_MAX) returned %s, errno = %d (%s)\n",
               huge ? "non-NULL (!)" : "NULL", saved, strerror(saved));
        free(huge);

        unsigned char *a = malloc(64);
        if (a == NULL) {
            fprintf(stderr, "m02-e02-lookup-malloc: out of memory\n");
            return EXIT_OOM;
        }
        memset(a, 0x11, 64);
        free(a);
        unsigned char *b = malloc(64); /* likely the same chunk, recycled */
        if (b == NULL) {
            fprintf(stderr, "m02-e02-lookup-malloc: out of memory\n");
            return EXIT_OOM;
        }
        printf("recycled block, bytes 16..23 (indeterminate!):");
        for (int i = 16; i < 24; i++)
            printf(" %02x", b[i]);
        printf("\n");
        free(b);
    }
    return checks_done("m02-e02-lookup-malloc");
}
