/* M04 e03: did it move? Complete program. Draw both branches at TRACE 2 and 3. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"

struct buf { char *data; size_t len, cap; };

static int grow(struct buf *b, size_t newcap, int *moved)
{
    uintptr_t before = (uintptr_t)b->data;
    char *tmp = realloc(b->data, newcap);
    if (tmp == NULL)
        return -1;
    *moved = (uintptr_t)tmp != before;
    b->data = tmp;
    b->cap = newcap;
    return 0;
}

int main(void)
{
    struct buf b = { malloc(12), 0, 12 };
    if (b.data == NULL) {
        fprintf(stderr, "m04-e03-trace-move: out of memory\n");
        return EXIT_OOM;
    }
    memcpy(b.data, "hello world", 12);   /* 11 chars + NUL */
    b.len = 11;

    const char *saved_ptr = b.data + 6;      /* points at "world" in block B1 */
    size_t saved_off = 6;
    (void)saved_ptr;                         /* never dereferenced after a grow */
    /* TRACE 1 */

    int moved1 = 0, moved2 = 0;
    if (grow(&b, 24, &moved1) != 0) {
        free(b.data);
        fprintf(stderr, "m04-e03-trace-move: out of memory\n");
        return EXIT_OOM;
    }
    /* TRACE 2 */
    printf("grow to 24: %s\n", moved1 ? "moved" : "in place");

    if (grow(&b, 1 << 20, &moved2) != 0) {
        free(b.data);
        fprintf(stderr, "m04-e03-trace-move: out of memory\n");
        return EXIT_OOM;
    }
    /* TRACE 3 */
    printf("grow to 1 MiB: %s\n", moved2 ? "moved" : "in place");

    check(strcmp(b.data + saved_off, "world") == 0, "the offset survives every move");
    free(b.data);
    return checks_done("m04-e03-trace-move");
}
