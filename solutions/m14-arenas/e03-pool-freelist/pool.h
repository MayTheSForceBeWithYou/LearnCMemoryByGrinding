#ifndef POOL_H
#define POOL_H
#include <stddef.h>

/* A fixed-size object pool: one block of count slots, each at least
 * objsize bytes. Free slots are threaded into a singly linked free list
 * that lives inside the free slots themselves. */
typedef struct {
    unsigned char *block; /* owned: count * slot bytes */
    size_t slot;          /* slot size in bytes (see pool_init) */
    size_t count;
    void *free_list;      /* first free slot, or NULL */
} Pool;

/* Prepares count slots of at least objsize bytes. The slot size is objsize
 * rounded up so that every slot can hold a pointer and every slot is
 * aligned to _Alignof(max_align_t). Returns 0, or -1 if objsize or count
 * is 0, if the sizes overflow, or on allocation failure. Exactly one malloc. */
int pool_init(Pool *p, size_t objsize, size_t count);

/* Returns a free slot in O(1), or NULL when none is left. Never calls malloc. */
void *pool_get(Pool *p);

/* Returns obj (from pool_get on this pool) to the pool in O(1). Accepts NULL. */
void pool_put(Pool *p, void *obj);

/* Frees the block. Every object from the pool becomes invalid. */
void pool_destroy(Pool *p);

#endif
