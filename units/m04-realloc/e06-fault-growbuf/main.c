#include <stdint.h>
#include <string.h>
#include "course.h"
#include "bbuf.h"

static int fail(struct bbuf *b)
{
    bbuf_free(b);
    fprintf(stderr, "m04-e06-fault-growbuf: out of memory\n");
    return EXIT_OOM;
}

int main(void)
{
    struct bbuf b = {0};
    unsigned char chunk[300];
    for (size_t i = 0; i < sizeof chunk; i++)
        chunk[i] = (unsigned char)i;

    if (bbuf_append(&b, NULL, 0) != 0)
        return fail(&b);
    check(b.len == 0, "empty append does nothing");

    size_t sizes[] = { 1, 7, 0, 64, 300, 3, 200 };
    size_t total = 0;
    for (size_t k = 0; k < sizeof sizes / sizeof sizes[0]; k++) {
        size_t before_len = b.len, before_cap = b.cap;
        if (bbuf_append(&b, chunk, sizes[k]) != 0) {
            check(b.len == before_len && b.cap == before_cap, "failed append leaves *b unchanged");
            return fail(&b);
        }
        total += sizes[k];
    }
    check(b.len == total, "len is the sum of appends");
    check(b.data[0] == 0 && b.data[1] == 0 && b.data[8] == 0 && b.data[8 + 63] == 63, "contents");
    check(b.data[total - 1] == 199, "last byte");

    size_t before = b.len;
    bbuf_shrink_to_fit(&b);                    /* failure is not fatal: */
    check(b.len == before && b.cap >= b.len, "shrink keeps contents, whatever it returns");
    check(b.data[total - 1] == 199, "contents after shrink");

    if (bbuf_append(&b, chunk, 10) != 0)
        return fail(&b);
    check(b.len == total + 10 && b.data[total + 9] == 9, "append after shrink");

    check(bbuf_reserve(&b, SIZE_MAX) == -1, "len + extra overflow is rejected");
    check(b.len == total + 10, "overflowing reserve leaves *b unchanged");

    bbuf_free(&b);
    check(b.data == NULL && b.len == 0 && b.cap == 0, "free resets");
    bbuf_shrink_to_fit(&b);                    /* empty: must not realloc(p, 0) */
    check(b.data == NULL && b.cap == 0, "shrinking an empty buffer leaves data NULL");
    return checks_done("m04-e06-fault-growbuf");
}
