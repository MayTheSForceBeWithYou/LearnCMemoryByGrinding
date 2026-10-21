/* M02 lesson demo: when does MALLOC_PERTURB_ touch recycled memory?
 *
 * Reads freed-then-reused memory on purpose, so it is plain-build only. Never do this in
 * real code: the bytes are indeterminate. glibc-specific (tcache, bins, mmap threshold).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Bytes 32..47: past the first 32 bytes, where glibc may store free-list links, the tcache
 * key, and large-bin links while the block is free. */
static void show(const char *label, const unsigned char *p, uintptr_t old)
{
    printf("%-28s %s address  bytes 32..47:", label, (uintptr_t)p == old ? "same" : "new ");
    for (int i = 32; i < 48; i++)
        printf(" %02x", p[i]);
    printf("\n");
}

int main(void)
{
    const char *env = getenv("MALLOC_PERTURB_");
    printf("MALLOC_PERTURB_=%s\n", env ? env : "(unset)");

    /* Small: 64 bytes is tcache-sized. free() parks it in the per-thread cache, and the next
     * malloc(64) takes it straight back out. */
    unsigned char *a = malloc(64);
    if (a == NULL)
        return 3;
    memset(a, 0x11, 64);
    uintptr_t old_a = (uintptr_t)a;   /* an integer, so no stale pointer survives the free */
    free(a);
    a = malloc(64);
    if (a == NULL)
        return 3;
    show("malloc(64), reused:", a, old_a);

    /* Large: 4096 bytes is above the tcache (1032) and fastbin limits and far below the
     * 128 KiB mmap threshold. The guard keeps it from merging into the top chunk on free. */
    unsigned char *b = malloc(4096);
    unsigned char *guard = malloc(16);
    if (b == NULL || guard == NULL)
        return 3;
    memset(b, 0x22, 4096);
    uintptr_t old_b = (uintptr_t)b;
    free(b);
    b = malloc(4096);
    if (b == NULL)
        return 3;
    show("malloc(4096), reused:", b, old_b);

    free(a);
    free(b);
    free(guard);
    return 0;
}
