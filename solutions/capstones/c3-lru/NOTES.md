# c3-lru: notes

## Part 1: the three bugs

Each was confirmed on its own by putting it back into the fixed solution alone.

**1. Eviction leaves the node in its hash chain (use after free).** `evict_lru` unlinked
the victim from the recency list and freed it, but never called `chain_unlink`. The bucket
still pointed to freed memory, and the next lookup in that bucket walked through it. ASan
reports `heap-use-after-free` in the chain walk, freed by `evict_lru`. The plain build
segfaults. Fix: `chain_unlink(c, victim)` before freeing.

**2. Replacing a value leaks the old one.** `lru_put` on an existing key stored the new
copy over `n->value` without freeing the old string. Only LSan at exit sees it:
`Direct leak of 2 byte(s)` from the `strdup` of `"3"`. Fix: `free(n->value)` after the new
copy succeeded, never before, so a failed `strdup` leaves the old value in place.

**3. Moving the tail to the front forgets `tail` (logic).** `touch` unlinked by hand and
did not update `c->tail` when `n` was the tail. The list then had `tail` pointing at the
node just moved to the front, so the next eviction removed the *most* recently used entry.
No tool reports it: the checks fail with `b was evicted` / `a, c, d remain`. Fix: use the
same `list_unlink` that maintains `head` and `tail`.

Bug 1 masks bug 2: ASan stops at the use after free, before LSan runs at exit.

## Part 2

`lru_resize` allocates the new bucket table **first** and returns -1 if that fails, before
touching anything. Everything after that (evicting down to the new capacity, relinking
every node into the new table, freeing the old table) cannot fail. That ordering is the
whole proof of "unchanged on failure". The table is only reallocated when its size changes.
Shrinking from 4 to 2 keeps 8 buckets (the minimum).

`lru_keys` walks from `head` and stores borrowed key pointers. It neither allocates nor
changes recency.

**Sweep.** `make part1` runs 618 fault points and `make check` runs 933. Every one exits 0 or 3
with no sanitizer report.
