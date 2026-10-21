# m09-e03-growth-count: answer key

Measured with `make build-fault && FA_COUNT=1 ./build/fault/prog 2>&1 | cat` on the shipped source:

```
failalloc: 1008 allocations
doubling:    len 1000  cap 1024
grow-by-one: len 1000  cap 1000
m09-e03-growth-count: all checks passed
```

(The `failalloc` line is on stderr. It appears first here because stdout is a pipe, so it is fully buffered and
flushed only at exit, after `failalloc` has written its line. Run on a terminal, stdout is line-buffered and the
`failalloc` line comes last; both orders were observed on the course machine.)

1. **Doubling:** 8 calls (capacities 8, 16, 32, 64, 128, 256, 512, 1024). **Grow-by-one:** 1000 calls.
2. **Total:** 1008. `failalloc` counts `malloc`, `calloc`, `realloc`, `reallocarray`, `strdup` and `strndup`
   calls. It never counts `free`, so the two `free` calls at the end add nothing.
3. **Worst-case copying for grow-by-one:** `0 + 1 + ... + 999 = 999·1000/2 = 499,500` int copies.
   It runs fast anyway because glibc can usually extend the block in place when the space after it is
   free, so most `realloc` calls copy nothing. The two vectors' reallocs are interleaved, so the space after the
   grow-by-one block is not always free: a copy of this loop that compared addresses saw the grow-by-one block
   move on 7 of its 999 resizing `realloc` calls on the course machine (plain glibc 2.44).
   The *call* count is what's certain. The copying cost depends on the allocator, and under ASan, which
   always moves the block on `realloc`, the worst case is what you get.
