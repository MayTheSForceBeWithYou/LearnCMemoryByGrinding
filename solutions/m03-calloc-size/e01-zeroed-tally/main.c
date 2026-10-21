/* M03 e01: zeroed tally. Write the three definitions below the marked line.
 * Until you do, the build fails at link time with "undefined reference". */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* New array of `buckets` counters, all 0, built with calloc. Caller frees.
 * NULL if buckets == 0 or allocation fails. */
size_t *tally_new(size_t buckets);
/* Same contract, built with malloc + memset; rejects an overflowing size itself. */
size_t *tally_new_memset(size_t buckets);
/* t[data[i] % buckets]++ for each byte. */
void tally_add(size_t *t, size_t buckets, const unsigned char *data, size_t len);

int main(void)
{
    const unsigned char text[] = "the quick brown fox jumps over the lazy dog";
    size_t len = sizeof text - 1;
    size_t nb = 7;

    size_t *a = tally_new(nb);
    size_t *b = tally_new_memset(nb);
    if (a == NULL || b == NULL) {
        free(a);
        free(b);
        fprintf(stderr, "m03-e01-zeroed-tally: out of memory\n");
        return EXIT_OOM;
    }
    tally_add(a, nb, text, len);
    tally_add(b, nb, text, len);
    size_t sum = 0;
    for (size_t i = 0; i < nb; i++)
        sum += a[i];
    check(sum == len, "every byte counted exactly once");
    check(memcmp(a, b, nb * sizeof *a) == 0, "both constructors give the same counts");
    size_t expect6 = 0;
    for (size_t i = 0; i < len; i++)
        expect6 += (text[i] % nb == 6);
    check(a[6] == expect6, "bucket 6");
    free(a);
    free(b);

    check(tally_new(0) == NULL, "buckets == 0 is rejected (calloc)");
    check(tally_new_memset(0) == NULL, "buckets == 0 is rejected (memset)");
    check(tally_new_memset(SIZE_MAX / 4) == NULL, "overflowing size is rejected (memset)");
    /* (SIZE_MAX/8 + 2) * 8 wraps to 8: an unchecked malloc would succeed with 8 bytes. */
    check(tally_new_memset(SIZE_MAX / 8 + 2) == NULL, "size that wraps to 8 bytes is rejected (memset)");
    check(tally_new(SIZE_MAX / 4) == NULL, "overflowing size is rejected (calloc)");
    return checks_done("m03-e01-zeroed-tally");
}

/* ---- your definitions below ---- */

size_t *tally_new(size_t buckets)
{
    if (buckets == 0)
        return NULL;
    return calloc(buckets, sizeof(size_t));
}

size_t *tally_new_memset(size_t buckets)
{
    if (buckets == 0 || buckets > SIZE_MAX / sizeof(size_t))
        return NULL;
    size_t *t = malloc(buckets * sizeof *t);
    if (t == NULL)
        return NULL;
    memset(t, 0, buckets * sizeof *t);
    return t;
}

void tally_add(size_t *t, size_t buckets, const unsigned char *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
        t[data[i] % buckets]++;
}
