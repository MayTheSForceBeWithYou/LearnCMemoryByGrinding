# M03 e01: Zeroed tally `[WRITE]`

## Goal

Implement a byte-frequency histogram whose counters start at zero:

```c
size_t *tally_new(size_t buckets);                       /* all counters 0 */
void    tally_add(size_t *t, size_t buckets, const unsigned char *data, size_t len);
size_t *tally_new_memset(size_t buckets);                /* same contract, built with malloc + memset */
```

`tally_add` increments `t[data[i] % buckets]` for each byte. The harness compares both
constructors on the same data and checks the counts. It also asks `tally_new_memset` for an
absurd number of buckets, including one whose byte count wraps around to a small number, and
expects NULL. A product that wraps to 8 bytes is the dangerous case: an unchecked `malloc` would
succeed with 8 bytes. It also asks `tally_new` for an overflowing count and expects `calloc` to say no.

**The starter will not build until you write the three definitions.** The link error is intentional.

## Constraints

- `tally_new` uses `calloc`. `tally_new_memset` uses `malloc` and `memset`, and must
  reject an overflowing size itself, with the same NULL contract as `calloc`.
- `buckets == 0` is invalid. Both constructors return NULL for it, and `tally_add` is never
  called with it.
- **Grading tool: Valgrind.** A counter that isn't zeroed is an uninitialised read, which ASan
  cannot see. `make valgrind` must report no "uninitialised value" errors and no leaks.

## How to verify

- `make check`: checks, ASan, and Valgrind clean.
- Try it: break your zeroing on purpose once and confirm that `make asan` stays quiet while
  `make valgrind` complains. That's the point of this exercise.
