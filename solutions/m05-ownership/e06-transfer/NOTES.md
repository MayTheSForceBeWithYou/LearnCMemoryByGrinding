# M05 e06 notes

- The only fallible step is growing the ring, and it comes *before* `msg` is stored. On failure we return without touching `msg`, which matches
  "ownership stays with the caller". If the function freed `msg` on failure, the harness's own `free(m)` would be a double free, and the fault sweep finds that.
- Growing a ring with `realloc` would keep the wrapped layout and need fix-up moves, so a fresh `malloc` plus an unwrap copy is simpler.
  The cost is one more allocation, and the old array is freed only after the copy succeeds.
- `pop` transfers by returning the pointer and forgetting it (head advances). `clear` frees only what the queue still owns.
- Counting: 40 messages pushed (m0..m39). Pops: m0, then 12 in the loop (i = 6, 9, ..., 39 pops m1..m12), then m13. So 40 - 14 = 26 remain
  for `queue_clear` to free.
