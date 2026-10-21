# m09-e06-vec-under-fire: notes

All capacity changes go through one helper that uses `reallocarray` into `tmp`. `vec_reserve` checks
`len + extra` for overflow before it allocates (the harness's `SIZE_MAX - 5` case). `vec_push` is
`reserve(1)` and a store. `vec_shrink_to_fit` ignores the helper's failure, because the old buffer is still
valid and still large enough. With `len == 0` it frees and resets instead of calling `realloc(p, 0)`.

**Sweep.** `verify-all` reports `sweep N=7 pass=1 oom=6 fail=0`. Six injected failures land on growth and
exit 3 after the harness confirms the vector is intact. One lands on the shrink (40+200 capacity down to 30),
which the contract absorbs, so that run *passes*. That pass is the evidence the shrink is correct.

**The usual mistake.** Reporting a failed shrink as OOM, or (worse) setting `cap = len` before knowing whether the
`realloc` succeeded. Then `cap` lies, and the next push writes past the real buffer.
