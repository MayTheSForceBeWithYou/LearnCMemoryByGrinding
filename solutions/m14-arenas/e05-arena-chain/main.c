/* m14-e05-arena-chain [FAULT]: reference solution. */
#include <stdlib.h>
#include <stdalign.h>
#include <stdint.h>
#include <string.h>
#include "chain.h"
#include "course.h"


#define ALIGN alignof(max_align_t)

void chain_init(Chain *c, size_t block_size)
{
    c->head = NULL;
    c->block_size = block_size;
}

void *chain_alloc(Chain *c, size_t size)
{
    if (size == 0 || size > SIZE_MAX - (ALIGN - 1))
        return NULL;
    size_t rounded = (size + ALIGN - 1) & ~(size_t)(ALIGN - 1);

    Block *b = c->head;
    if (!b || rounded > b->cap - b->used) {
        size_t cap = rounded > c->block_size ? rounded : c->block_size;
        if (cap > SIZE_MAX - sizeof(Block))
            return NULL;
        b = malloc(sizeof(Block) + cap);
        if (!b)
            return NULL;
        b->cap = cap;
        b->used = 0;
        b->next = c->head;
        c->head = b;
    }
    void *p = (unsigned char *)b->data + b->used;
    b->used += rounded;
    return p;
}

void chain_destroy(Chain *c)
{
    Block *b = c->head;
    while (b) {
        Block *next = b->next;
        free(b);
        b = next;
    }
    c->head = NULL;
}

static size_t blocks(const Chain *c)
{
    size_t n = 0;
    for (const Block *b = c->head; b; b = b->next)
        n++;
    return n;
}

int main(void)
{
    Chain c;
    chain_init(&c, 64);
    check(c.head == NULL, "init allocates nothing");

    char *first = NULL;
    for (int i = 0; i < 10; i++) {
        char *s = chain_alloc(&c, 20);
        if (!s)
            goto oom;
        memset(s, 'a' + i, 20);
        if (i == 0)
            first = s;
        check((uintptr_t)s % alignof(max_align_t) == 0, "aligned");
    }
    check(blocks(&c) == 5, "64-byte blocks hold two rounded 20-byte objects each");
    check(first[0] == 'a' && first[19] == 'a', "early allocations are intact");

    double *big = chain_alloc(&c, 1000 * sizeof *big);
    if (!big)
        goto oom;
    big[999] = 1.0;
    check(blocks(&c) == 6, "an oversized request gets its own block");
    check(chain_alloc(&c, 0) == NULL, "size 0 gives NULL");
    check(chain_alloc(&c, SIZE_MAX) == NULL, "SIZE_MAX gives NULL");

    chain_destroy(&c);
    check(c.head == NULL, "destroy empties the chain");
    return checks_done("m14-e05-arena-chain");
oom:
    chain_destroy(&c);
    fprintf(stderr, "m14-e05: out of memory\n");
    return EXIT_OOM;
}
