# m14-e04-heap-usage: notes

Real output (`capture.txt`, `make capture TOOL=usage`):

```
### list
sum = 499500
==797732==     in use at exit: 0 bytes in 0 blocks
==797732==   total heap usage: 1,001 allocs, 1,001 frees, 20,096 bytes allocated
### arena
sum = 499500
==797744==     in use at exit: 0 bytes in 0 blocks
==797744==   total heap usage: 2 allocs, 2 frees, 20,096 bytes allocated
```

**The bytes allocated are identical**: 1000 × 16 = 16000 for the nodes, plus 4096 for
stdout's buffer, allocated by the first `printf` (the extra allocation in each mode).
Valgrind sums *requested* sizes, so the summary hides the per-chunk cost.

**Real footprint.** Each `malloc(16)` in glibc is a 32-byte chunk (16 bytes of header and
rounding per node), so list mode occupies about 32000 bytes of heap for its nodes. The
single `malloc(16000)` is one chunk of 16016 bytes. The arena uses half the memory, and its
nodes are contiguous, which is better for the cache in the summing loop. No line of the
Valgrind summary shows this. The `allocs` count is the only hint: 1001 vs 2.

**Lesson.** "Bytes allocated" counts requests, not footprint. To see the footprint, use
glibc's `malloc_stats()` or `mallinfo2()`, or Valgrind's `massif` tool, which adds a per-block
admin estimate (`--heap-admin`, 8 bytes by default).
