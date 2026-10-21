#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

int queue_push_owned(struct queue *q, char *msg)
{
    if (q->len == q->cap) {
        size_t nc = q->cap ? q->cap * 2 : 4;
        if (nc > SIZE_MAX / sizeof *q->items)
            return -1;
        char **tmp = malloc(nc * sizeof *tmp);   /* new ring, unwrapped */
        if (tmp == NULL)
            return -1;                           /* msg untouched: still the caller's */
        for (size_t i = 0; i < q->len; i++)
            tmp[i] = q->items[(q->head + i) % q->cap];
        free(q->items);
        q->items = tmp;
        q->head = 0;
        q->cap = nc;
    }
    q->items[(q->head + q->len) % q->cap] = msg;  /* the transfer happens here */
    q->len++;
    return 0;
}

char *queue_pop(struct queue *q)
{
    if (q->len == 0)
        return NULL;
    char *m = q->items[q->head];
    q->head = (q->head + 1) % q->cap;
    q->len--;
    return m;
}

void queue_clear(struct queue *q)
{
    for (size_t i = 0; i < q->len; i++)
        free(q->items[(q->head + i) % q->cap]);
    free(q->items);
    memset(q, 0, sizeof *q);
}
