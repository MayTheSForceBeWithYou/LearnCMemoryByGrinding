# m09-e01-vec-int: notes

**Why it is correct.** All growth goes through one helper, which computes the new capacity with an explicit
`cap > SIZE_MAX / 2` guard and lets `reallocarray` check `cap * sizeof(int)`. The result lands in `tmp`, and
`data` and `cap` change only on success, so a failed push leaves the vector exactly as it was. `vec_destroy` resets
to `{NULL, 0, 0}`, which makes a second destroy a `free(NULL)`.

**Numbers.** 200,000 pushes from capacity 8 with doubling take 16 allocation calls (8, 16, ..., 262144).
`FA_COUNT=1 ./build/fault/prog` on this solution prints `failalloc: 17 allocations`: those 16, plus the
`reallocarray` call in the byte-overflow case, which the wrapper counts even though it returns NULL.

**Alternatives.** ×1.5 growth wastes less memory and can reuse freed space better. Any factor of at least 1.25
passes; the harness checks every growth for `new >= old + old / 4`, so a constant step such as `+4096` fails long
before the 200,000th push.
`#define _DEFAULT_SOURCE` is needed for `reallocarray` under `-std=c17`. Without it, write the
`SIZE_MAX / sizeof *v->data` check yourself and call `realloc`.

**The usual mistake.** `v->data = realloc(v->data, ...)`, which loses the block on failure, or updating `cap`
before the `realloc` succeeds.
