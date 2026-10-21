# M06 e04 notes

N = 8: four calls per cache (`calloc` struct, `strdup` name, `calloc` slots, `malloc` fast), times two caches. Note that `strdup` and `calloc`
count, because the injector wraps them, just like `malloc`. stdio's buffer for `printf` is allocated inside libc, and `--wrap`
only rewrites calls in our own object files, so it isn't counted.

| FAIL_AT | call | exit |
|---|---|---|
| 1 | a: struct | 3 |
| 2 | a: name | 3 |
| 3 | a: slots | 3 |
| 4 | a: fast (optional) | **0** |
| 5 | b: struct | 3 |
| 6 | b: name | 3 |
| 7 | b: slots | 3 |
| 8 | b: fast (optional) | **0** |

Summary: `fault-sweep: N=8 pass=2 oom=6 fail=0`.

- The usual miss: assuming `FAIL_AT=2`, the name, can't happen independently of 3. `cache_create` makes *both* calls before checking, so
  when the name fails, slots is still allocated, and `cache_destroy` frees it. Calling both and then checking both is legitimate here, because
  the destroy is NULL-safe.
- `b` is created only if `a` succeeded (`a ? ... : NULL`), so in runs 1-3 the count stops early. That doesn't change which call
  number fails, but it means those runs make fewer than 8 calls.
