#ifndef CHAIN_H
#define CHAIN_H
#include <stddef.h>

/* One malloc'd block. data is max_align_t-aligned because the flexible
 * array member has type max_align_t. */
typedef struct Block {
    struct Block *next; /* older block, or NULL */
    size_t cap;         /* usable bytes in data */
    size_t used;
    max_align_t data[];
} Block;

/* A growable bump arena: a chain of blocks, newest first. */
typedef struct {
    Block *head;       /* owned chain */
    size_t block_size; /* usable bytes in a normal block */
} Chain;

/* Makes c empty. Allocates nothing. block_size must be at least 1. */
void chain_init(Chain *c, size_t block_size);

/* Returns size bytes aligned to _Alignof(max_align_t), from the newest block
 * if it fits, otherwise from a new block of block_size usable bytes (or of
 * exactly the rounded size, if that is larger). Returns NULL if size is 0,
 * if sizes overflow, or on allocation failure; c stays valid and keeps every
 * earlier allocation. */
void *chain_alloc(Chain *c, size_t size);

/* Frees every block. c is empty afterwards and can be reused. */
void chain_destroy(Chain *c);

#endif
