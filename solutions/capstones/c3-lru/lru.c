/* c3-lru: reference solution (Part 1 fixed, Part 2 added). */
#define _POSIX_C_SOURCE 200809L
#include "lru.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *key;           /* owned */
    char *value;         /* owned */
    struct Node *hnext;  /* next node in the same bucket */
    struct Node *prev;   /* towards the most recently used end */
    struct Node *next;   /* towards the least recently used end */
} Node;

struct Lru {
    Node **buckets;      /* owned table of nbuckets chains */
    size_t nbuckets;     /* power of two */
    size_t len, capacity;
    Node *head;          /* most recently used, or NULL */
    Node *tail;          /* least recently used, or NULL */
};

/* FNV-1a, 64-bit. */
static uint64_t hash(const char *s)
{
    uint64_t h = 14695981039346656037u;
    for (; *s; s++) {
        h ^= (unsigned char)*s;
        h *= 1099511628211u;
    }
    return h;
}

static size_t table_size(size_t capacity)
{
    size_t n = 8;
    while (n < capacity && n <= SIZE_MAX / 2)
        n *= 2;
    return n;
}

static Node **bucket_of(const Lru *c, const char *key)
{
    return &c->buckets[hash(key) & (c->nbuckets - 1)];
}

static Node *find(const Lru *c, const char *key)
{
    for (Node *n = *bucket_of(c, key); n; n = n->hnext)
        if (strcmp(n->key, key) == 0)
            return n;
    return NULL;
}

/* ---- recency list ---- */

static void list_unlink(Lru *c, Node *n)
{
    if (n->prev)
        n->prev->next = n->next;
    else
        c->head = n->next;
    if (n->next)
        n->next->prev = n->prev;
    else
        c->tail = n->prev;
    n->prev = n->next = NULL;
}

static void list_push_front(Lru *c, Node *n)
{
    n->prev = NULL;
    n->next = c->head;
    if (c->head)
        c->head->prev = n;
    c->head = n;
    if (!c->tail)
        c->tail = n;
}

static void touch(Lru *c, Node *n)
{
    if (c->head == n)
        return;
    list_unlink(c, n);
    list_push_front(c, n);
}

/* ---- hash chains ---- */

static void chain_unlink(Lru *c, Node *n)
{
    Node **pp = bucket_of(c, n->key);
    while (*pp != n)
        pp = &(*pp)->hnext;
    *pp = n->hnext;
    n->hnext = NULL;
}

static void node_free(Node *n)
{
    free(n->key);
    free(n->value);
    free(n);
}

static void evict_lru(Lru *c)
{
    Node *victim = c->tail;
    chain_unlink(c, victim);
    list_unlink(c, victim);
    node_free(victim);
    c->len--;
}

/* ---- public API ---- */

Lru *lru_create(size_t capacity)
{
    if (capacity == 0)
        return NULL;
    Lru *c = malloc(sizeof *c);
    if (!c)
        return NULL;
    c->nbuckets = table_size(capacity);
    c->buckets = calloc(c->nbuckets, sizeof *c->buckets);
    if (!c->buckets) {
        free(c);
        return NULL;
    }
    c->len = 0;
    c->capacity = capacity;
    c->head = c->tail = NULL;
    return c;
}

void lru_destroy(Lru *c)
{
    if (!c)
        return;
    Node *n = c->head;
    while (n) {
        Node *next = n->next;
        node_free(n);
        n = next;
    }
    free(c->buckets);
    free(c);
}

int lru_put(Lru *c, const char *key, const char *value)
{
    Node *n = find(c, key);
    if (n) {
        char *v = strdup(value);
        if (!v)
            return -1;
        free(n->value);
        n->value = v;
        touch(c, n);
        return 0;
    }

    n = malloc(sizeof *n);
    if (!n)
        return -1;
    n->key = strdup(key);
    n->value = strdup(value);
    if (!n->key || !n->value) {
        free(n->key);
        free(n->value);
        free(n);
        return -1;
    }
    if (c->len == c->capacity)
        evict_lru(c);
    Node **b = bucket_of(c, key);
    n->hnext = *b;
    *b = n;
    list_push_front(c, n);
    c->len++;
    return 0;
}

const char *lru_get(Lru *c, const char *key)
{
    Node *n = find(c, key);
    if (!n)
        return NULL;
    touch(c, n);
    return n->value;
}

int lru_remove(Lru *c, const char *key)
{
    Node *n = find(c, key);
    if (!n)
        return 0;
    chain_unlink(c, n);
    list_unlink(c, n);
    node_free(n);
    c->len--;
    return 1;
}

size_t lru_len(const Lru *c)
{
    return c->len;
}

/* ---- Part 2 ---- */

int lru_resize(Lru *c, size_t capacity)
{
    if (capacity == 0)
        return -2;
    size_t nb = table_size(capacity);
    Node **table = NULL;
    if (nb != c->nbuckets) {
        table = calloc(nb, sizeof *table);
        if (!table)
            return -1;
    }
    /* Nothing below can fail, so the cache is unchanged on -1. */
    while (c->len > capacity)
        evict_lru(c);
    c->capacity = capacity;
    if (table) {
        for (size_t i = 0; i < c->nbuckets; i++) {
            Node *n = c->buckets[i];
            while (n) {
                Node *next = n->hnext;
                Node **b = &table[hash(n->key) & (nb - 1)];
                n->hnext = *b;
                *b = n;
                n = next;
            }
        }
        free(c->buckets);
        c->buckets = table;
        c->nbuckets = nb;
    }
    return 0;
}

size_t lru_keys(const Lru *c, const char **out, size_t max)
{
    size_t k = 0;
    for (const Node *n = c->head; n && k < max; n = n->next)
        out[k++] = n->key;
    return k;
}
