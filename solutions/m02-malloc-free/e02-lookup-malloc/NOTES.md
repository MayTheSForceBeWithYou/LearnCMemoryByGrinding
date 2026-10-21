# M02 e02 notes

Answers, paraphrasing man-pages 6.x `malloc(3)` (quote the page itself in your own ANSWER.md):

1. `malloc(0)` returns either NULL or a unique pointer that can be passed to `free`. Both are safe to free. glibc returns
   a unique minimum-size chunk.
2. On error, it returns NULL and sets `errno` to `ENOMEM` (ERRORS section). The probe shows `errno = 12 (Cannot allocate memory)`.
3. NOTES/BUGS: by default Linux overcommits, so a non-NULL result does not guarantee the memory is
   available. The OOM killer may strike when the pages are first touched. Our NULL checks handle the failures you *can* see.
4. `free(NULL)`: "no operation is performed". Already freed: "undefined behavior occurs".
5. `MALLOC_PERTURB_`, `MALLOC_CHECK_` (glibc tunables, see `mallopt(3)` and `tunables(7)`). PERTURB fills freed memory with
   the given byte and newly allocated memory with its complement, which makes reads of stale or uninitialised memory
   visibly wrong. Note that in recent glibc the first 16 bytes of a freed tcache chunk hold bookkeeping, which is why the
   probe prints bytes 16..23.
6. `malloc`, `free`, `calloc`, `realloc`: C (C89 onward) and POSIX. `reallocarray`: glibc 2.26, OpenBSD, and POSIX.1-2024.
   (The exact wording of STANDARDS/HISTORY varies with the man-pages version.)

**The perturb surprise.** With only `MALLOC_PERTURB_` set, the recycled bytes usually still read `11`. That's
because small frees go to the per-thread **tcache**, and the tcache fast path skips the perturb fill. With
`glibc.malloc.tcache_count=0`, the chunk goes through the main free path, and the recycled block reads `5a`
(the complement of 0xa5 = 165). Confirmed on the course machine (Arch glibc).

The `peek` mode reads indeterminate bytes on purpose. That's why it isn't in `make check` (Valgrind would rightly flag it).
