/* c3-lru [FAULT] capstone harness.
 * `make part1` builds with -DLRU_PART1, which leaves out the Part 2 checks. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "lru.h"

static Lru *cache;

static void oom(void)
{
    lru_destroy(cache);
    fprintf(stderr, "c3-lru: out of memory\n");
    exit(EXIT_OOM);
}

static void put(const char *k, const char *v)
{
    if (lru_put(cache, k, v) != 0)
        oom();
}

static int value_is(const char *k, const char *want)
{
    const char *v = lru_get(cache, k);
    if (!want)
        return v == NULL;
    return v && strcmp(v, want) == 0;
}

static void part1(void)
{
    cache = lru_create(3);
    if (!cache)
        oom();
    check(lru_create(0) == NULL, "capacity 0 is rejected");
    put("a", "1");
    put("b", "2");
    put("c", "3");
    check(lru_len(cache) == 3, "three entries");
    check(value_is("a", "1"), "a is 1");   /* order now a, c, b */
    put("d", "4");                          /* evicts b */
    check(value_is("b", NULL), "b was evicted");
    check(value_is("a", "1") && value_is("c", "3") && value_is("d", "4"), "a, c, d remain");

    put("c", "three");                      /* replace; order c, d, a */
    check(value_is("c", "three") && lru_len(cache) == 3, "replace keeps len");
    put("e", "5");                          /* evicts a */
    check(value_is("a", NULL), "a was evicted after c was replaced");

    check(lru_remove(cache, "zz") == 0, "removing an absent key");
    check(lru_remove(cache, "d") == 1 && lru_len(cache) == 2, "d removed");
    check(value_is("d", NULL), "d is gone");

    /* Many keys through a small cache: lots of evictions. */
    char k[16], v[16];
    for (int i = 0; i < 200; i++) {
        snprintf(k, sizeof k, "k%d", i);
        snprintf(v, sizeof v, "v%d", i);
        put(k, v);
        if (i % 3 == 0)
            lru_get(cache, "k0");
    }
    check(lru_len(cache) == 3, "len never exceeds capacity");
    check(value_is("k199", "v199") && value_is("k198", "v198"), "newest keys survive");
    check(value_is("k0", NULL), "an old key is evicted once it stops being used");
    lru_destroy(cache);
    cache = NULL;
}

#ifndef LRU_PART1
static void part2(void)
{
    cache = lru_create(4);
    if (!cache)
        oom();
    put("a", "1");
    put("b", "2");
    put("c", "3");
    put("d", "4");
    lru_get(cache, "a"); /* order a, d, c, b */

    const char *keys[8];
    size_t n = lru_keys(cache, keys, 8);
    check(n == 4 && strcmp(keys[0], "a") == 0 && strcmp(keys[1], "d") == 0 &&
              strcmp(keys[3], "b") == 0, "lru_keys lists MRU first");
    check(lru_keys(cache, keys, 2) == 2, "lru_keys respects max");
    check(lru_resize(cache, 0) == -2, "capacity 0 is rejected");

    if (lru_resize(cache, 2) != 0)
        oom();
    check(lru_len(cache) == 2 && value_is("b", NULL) && value_is("c", NULL), "shrinking evicts LRU entries");
    check(value_is("a", "1") && value_is("d", "4"), "shrinking keeps MRU entries");

    if (lru_resize(cache, 100) != 0)
        oom();
    char k[16];
    for (int i = 0; i < 100; i++) {
        snprintf(k, sizeof k, "key%d", i);
        put(k, k);
    }
    check(lru_len(cache) == 100, "growing allows 100 entries");
    check(value_is("key0", "key0") && value_is("key99", "key99"), "lookups work after rehash");
    check(value_is("a", NULL), "a was evicted by the 100 new keys");
    lru_destroy(cache);
    cache = NULL;
}
#endif

int main(void)
{
    part1();
#ifndef LRU_PART1
    part2();
#endif
    return checks_done("c3-lru");
}
