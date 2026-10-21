# m10-e02-free-loop: notes

**The bug.** `playlist_destroy`'s `for` loop evaluates `t = t->next` *after* the body has run `free(t)`.
Every iteration after the first reads a freed `Track`. Fix: save `next` before freeing (this solution).

**Why the plain build "works".** `Track` is `{title, artist, seconds, next}`, so `next` is at offset 24. `sizeof(Track)` is 32,
so `malloc` hands out a 48-byte chunk (32 bytes plus the 8-byte header, rounded up to 16). When it is freed into
glibc's tcache, glibc overwrites offsets 0-15 (a mangled list pointer and
a key) and leaves `next` intact. On the course machine the plain build exited 0 on every run.

**MALLOC_PERTURB_.** Measured on the course machine (glibc 2.44):

| Plain run | Exit |
|---|---|
| no env | 0 |
| `MALLOC_PERTURB_=165` | 0 |
| `GLIBC_TUNABLES=glibc.malloc.perturb=165` | 0 |
| `GLIBC_TUNABLES=glibc.malloc.tcache_count=0 MALLOC_PERTURB_=165` | 139 (SIGSEGV) |

Perturbation applies only to chunks that skip the tcache. With the tcache disabled, the freed `Track` is filled
with `0xa5` bytes (the `MALLOC_PERTURB_` value itself; new allocations get its complement, `0x5a`), `t->next`
becomes a wild pointer made of `0xa5` bytes, and the loop crashes. Checked on the course machine with a small
program that read byte 24 of a 32-byte block: `0x5a` right after `malloc`, `0xa5` after `free` with the tcache
off, and `0x00` (not perturbed) with the tcache on.

**ASan** reports `heap-use-after-free ... READ of size 8` in `playlist_destroy` at the `for` line, with
"freed by" pointing at `free(t)` in the body. gcc 16 may also warn at compile time (`-Wuse-after-free`).
