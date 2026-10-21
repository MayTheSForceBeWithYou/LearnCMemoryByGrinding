#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

/* A FIFO of owned heap strings. {0} is a valid empty queue.
 * items is a ring buffer of cap slots; the oldest message is at items[head]. */
struct queue {
    char **items;
    size_t head, len, cap;
};

/* Takes ownership of msg (which must come from malloc/strdup) ON SUCCESS ONLY.
 * Returns 0: the queue owns msg; the caller must not use or free it again.
 * Returns -1 (allocation failure): ownership stays with the caller, and the queue
 * has not stored msg and is unchanged. */
int   queue_push_owned(struct queue *q, char *msg);
/* Removes the oldest message and transfers it to the caller (free()). NULL if empty. */
char *queue_pop(struct queue *q);
/* Frees every queued message and the storage; leaves q as {0}, reusable. */
void  queue_clear(struct queue *q);

#endif
