# m14-e03-pool-freelist: notes

**Slot size.** `max(objsize, sizeof(void *))`, rounded to 16, with overflow checks on the
rounding (`size > SIZE_MAX - 15`) and on `count * size` (`count > SIZE_MAX / size`). The
harness's `pool_init(&p, 8, SIZE_MAX / 4)` must hit the second check: slot 16 times
`SIZE_MAX / 4` wraps. `Particle` (two doubles and `char[9]`) is 32 bytes with padding, and
its slot is 32.

**Free list.** Built from the last slot to the first, so the head is slot 0 and `pool_get`
returns slots in address order (better locality for the first pass). `pool_get` pops and
`pool_put` pushes, so the list is LIFO: the most recently freed slot, likely still in cache, is
reused first.

**Storing the link.** `*(void **)s = next` writes a `void *` object into the slot's
storage. That is fine for memory from `malloc`: it has no declared type, so the store gives
those bytes the effective type `void *` (C17 6.5p6) until the user's object overwrites them.
The slot is aligned for `void *` because it is 16-aligned. `memcpy(s, &next, sizeof next)` is
the equivalent that needs no argument about effective types.

**What the tools miss.** After `pool_put(&p, a)`, the first 8 bytes of `a`'s slot hold the
free-list link, which is exactly where `a->x` lives. A stale `a->x = 1.0` overwrites the link
with the bits of a double, and the next `pool_get` after that returns a garbage pointer.
Neither ASan nor Memcheck says a word: the block is one live allocation.
