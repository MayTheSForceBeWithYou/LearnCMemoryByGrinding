# M04 e06 notes

- **One failure path.** `append` goes through `reserve`, so the only `realloc` for growth (and its `tmp` check) is in one place.
  `shrink_to_fit` has the second one.
- **Overflow:** `extra > SIZE_MAX - len` is the subtraction form of "`len + extra` wraps". It's exact and needs no division. The harness
  calls `bbuf_reserve(&b, SIZE_MAX)` to prove it never reaches the allocator (under ASan, an impossible request would abort).
- **Shrink:** a failed shrink is harmless (the old block is still valid), so it returns 1 rather than -1. The caller doesn't need to
  do anything. An empty buffer is freed, never `realloc(p, 0)`.
- **Sweep:** about 6 allocation calls (initial 16, grows to 32, 64, 512, 1024, the shrink, and one regrow). Each `FAIL_AT` makes one of them
  return NULL. `main` frees and exits 3 for an append failure, and the shrink failure is absorbed and the run passes. Both outcomes are allowed.
- Common mistake: calling `realloc` in `append` *and* `reserve`, and getting the commit order wrong in one of them.
