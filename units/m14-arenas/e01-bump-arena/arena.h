#ifndef ARENA_H
#define ARENA_H
#include <stddef.h>

typedef struct {
    unsigned char *base; /* owned block of cap bytes, from malloc */
    size_t cap;
    size_t used;         /* bytes handed out so far, including padding */
} Arena;

/* Allocates the arena's single block of cap bytes. Returns 0, or -1 on
 * allocation failure (a is then safe to pass to arena_free). */
int arena_init(Arena *a, size_t cap);

/* Returns size bytes from the arena, aligned to _Alignof(max_align_t),
 * or NULL if size is 0 or the request does not fit. Never calls malloc. */
void *arena_alloc(Arena *a, size_t size);

/* Forgets every allocation; the whole block is available again. */
void arena_reset(Arena *a);

/* Frees the block. Afterwards a is empty (base NULL, cap and used 0). */
void arena_free(Arena *a);

#endif
