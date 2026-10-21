# M02 e03 notes

- `b.view` was copied from `scores` while it held block B1 (8 ints, 32 bytes). `reset_scores` calls `free(old)`,
  which ends B1's lifetime, then allocates B2 and returns it. `scores` is updated. `b.view` still holds B1's address.
- The plain build reads B1's old bytes. glibc's tcache writes bookkeeping into the first 16 bytes of a freed chunk and
  often leaves the rest, and the allocator may even hand B1 back as B2 (same size class). In that case
  `b.view == scores`, and the check *passes by accident*. Either way, it's undefined behaviour.
- Under ASan, freed blocks sit in quarantine, so B2 is never B1. `board_total` triggers `heap-use-after-free`, with the free
  stack in `reset_scores` and the allocation stack in the first `reset_scores` call.
- `scores = NULL` inside `reset_scores` (or after the free) would change the variable `scores` only. The stale copy is
  `b.view`. The fix is to re-point the borrower after the owner replaced the block (or not store a borrow at all and pass
  `scores` to `board_total` each time).
