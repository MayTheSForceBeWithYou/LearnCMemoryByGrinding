/* m11-e01-shadow-bytes [REPORT]: read capture.txt before you run anything. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* Returns a heap string holding the lowercase hex encoding of src[0..n).
 * The caller frees it. Returns NULL on allocation failure. */
static char *hex_encode(const unsigned char *src, size_t n)
{
    static const char digits[] = "0123456789abcdef";
    if (n > (SIZE_MAX - 1) / 2)
        return NULL;
    char *out = malloc(n * 2 + 1);
    if (!out)
        return NULL;
    for (size_t i = 0; i < n; i++) {
        out[2 * i] = digits[src[i] >> 4];
        out[2 * i + 1] = digits[src[i] & 0xf];
    }
    out[2 * n] = '\0';
    return out;
}

int main(void)
{
    const unsigned char key[] = {0xde, 0xad, 0xbe, 0xef, 0x01, 0x23, 0x45};
    char *hex = hex_encode(key, sizeof key);
    if (!hex) {
        fprintf(stderr, "m11-e01: out of memory\n");
        return EXIT_OOM;
    }
    check(strcmp(hex, "deadbeef012345") == 0, "hex_encode encodes 7 bytes");
    free(hex);
    return checks_done("m11-e01-shadow-bytes");
}
