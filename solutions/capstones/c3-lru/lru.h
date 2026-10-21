#ifndef LRU_H
#define LRU_H
#include <stddef.h>

/* An LRU cache from string keys to string values.
 *
 * Ownership: the cache owns private copies of every key and value. Strings
 * passed in are only read during the call. Pointers handed out are
 * borrowed: they stay valid until the next call that modifies the cache
 * (lru_put, lru_remove, lru_resize, lru_destroy). lru_get modifies the
 * recency order but no strings, so it does not invalidate borrowed pointers.
 *
 * Allocation failure: every function that can allocate returns -1 (or
 * NULL) on failure and leaves the cache exactly as it was. */
typedef struct Lru Lru;

/* Creates an empty cache holding at most capacity entries (capacity >= 1).
 * Returns NULL if capacity is 0 or on allocation failure. */
Lru *lru_create(size_t capacity);

/* Frees the cache with every key and value. Accepts NULL. */
void lru_destroy(Lru *c);

/* Stores a copy of value under key and makes key the most recently used.
 * If key is new and the cache is full, the least recently used entry is
 * evicted first. Returns 0, or -1 on allocation failure. */
int lru_put(Lru *c, const char *key, const char *value);

/* Returns the value for key (borrowed) and makes key the most recently
 * used, or returns NULL if key is absent. */
const char *lru_get(Lru *c, const char *key);

/* Removes key. Returns 1 if it was present, 0 if not. */
int lru_remove(Lru *c, const char *key);

/* Number of entries. */
size_t lru_len(const Lru *c);

/* ---- Part 2 ------------------------------------------------------------ */

/* Changes the capacity (>= 1). Shrinking evicts least recently used
 * entries until len <= capacity. The bucket table is resized to the
 * smallest power of two >= capacity, never below 8. Returns 0, -1 on
 * allocation failure (cache unchanged), or -2 if capacity is 0. */
int lru_resize(Lru *c, size_t capacity);

/* Stores up to max borrowed key pointers in out, most recently used
 * first, and returns how many it stored. Does not change recency. */
size_t lru_keys(const Lru *c, const char **out, size_t max);

#endif
