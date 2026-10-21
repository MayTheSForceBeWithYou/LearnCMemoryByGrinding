#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "queue.h"

static int push_text(struct queue *q, const char *text, char **pushed)
{
    char *m = strdup(text);
    if (m == NULL)
        return -1;
    if (queue_push_owned(q, m) != 0) {
        free(m);               /* contract: failure leaves ownership with us */
        return -1;
    }
    if (pushed)
        *pushed = m;           /* remembered only to compare addresses, never used */
    return 0;
}

static int oom(struct queue *q)
{
    queue_clear(q);
    fprintf(stderr, "m05-e06-transfer: out of memory\n");
    return EXIT_OOM;
}

int main(void)
{
    struct queue q = {0};
    char *first = NULL;
    char text[16];

    if (push_text(&q, "m0", &first) != 0)
        return oom(&q);
    for (int i = 1; i < 6; i++) {
        snprintf(text, sizeof text, "m%d", i);
        if (push_text(&q, text, NULL) != 0)
            return oom(&q);
    }
    char *p = queue_pop(&q);
    check(p == first, "pop returns the very block that was pushed");
    check(p != NULL && strcmp(p, "m0") == 0, "FIFO order");
    free(p);

    /* Interleave to wrap the ring around. */
    for (int i = 6; i < 40; i++) {
        snprintf(text, sizeof text, "m%d", i);
        if (push_text(&q, text, NULL) != 0)
            return oom(&q);
        if (i % 3 == 0) {
            p = queue_pop(&q);
            free(p);
        }
    }
    p = queue_pop(&q);
    check(p != NULL && strcmp(p, "m13") == 0, "order survives wrap-around and growth");
    free(p);
    check(q.len == 26, "remaining count");

    queue_clear(&q);           /* frees the 26 queued messages */
    check(q.items == NULL && q.len == 0, "clear leaves {0}");
    if (push_text(&q, "again", NULL) != 0)
        return oom(&q);
    p = queue_pop(&q);
    check(p != NULL && strcmp(p, "again") == 0, "reusable after clear");
    free(p);
    check(queue_pop(&q) == NULL, "empty pop is NULL");
    queue_clear(&q);
    return checks_done("m05-e06-transfer");
}
