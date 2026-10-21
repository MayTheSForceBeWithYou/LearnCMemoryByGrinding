# c3-lru `[FAULT]` capstone: an LRU cache

## Goal

`lru.c` implements the string-to-string LRU cache specified in `lru.h`. It uses a hash
table of chains (for O(1) lookup) and a doubly linked recency list (for O(1)
move-to-front and eviction). Each entry is one `Node` on both structures. You get it
**with three bugs**, the way you would inherit code at work.

### Part 1: fix it

```
make part1     # builds with -DLRU_PART1: Part 1 checks only, all stages
```

`make part1` must pass: checks, ASan/LSan, Valgrind and the fault sweep. The three bugs
show up in three different ways: one as a wrong answer from the checks, one as an ASan
report, one only at exit. Fixing one can unmask another. Write `ANSWER.md` with one section
per bug: symptom, the tool output that located it, mechanism, fix.

### Part 2: extend it

Implement `lru_resize` and `lru_keys` as specified in `lru.h`. Until you do, `make check`
fails to link, on purpose. `lru_resize` must rehash into a new bucket table and must leave
the cache *exactly* unchanged when that allocation fails, so decide what can fail before
you change anything.

## Constraints

- Keep the design: one `Node` per entry on both structures, and no second allocation per
  lookup. `lru_get` allocates nothing.
- Every function that allocates honours the "cache unchanged on failure" contract. The
  sweep tests every allocation point, a few hundred of them.
- Do not change `lru.h` or `main.c`.

## Verify

`make part1` passes, then `make check` passes (Part 1 and Part 2, every stage).

<details><summary>Hint</summary>

For every operation that unlinks a node, list the pointers that refer to it: the bucket
chain, the neighbours' `prev`/`next`, and `head`/`tail`. Every one must be updated.
</details>
