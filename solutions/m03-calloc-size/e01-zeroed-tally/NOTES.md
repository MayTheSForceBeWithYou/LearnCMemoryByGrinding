# M03 e01 notes

- `calloc(buckets, sizeof(size_t))` checks the multiplication and zeroes. `sizeof *t` would need a variable first. Either is fine.
- `tally_new_memset` must replicate *both* of `calloc`'s guarantees: the `SIZE_MAX / sizeof` pre-check, then `memset` of
  exactly the same byte count.
- The instructive failure: drop the `memset`. The plain build may still pass (fresh pages from the kernel are zero), ASan
  stays silent (ASan doesn't track initialisation, and actually fills new blocks with a non-zero pattern, which may
  make the *checks* fail), and Valgrind reports "Conditional jump or move depends on uninitialised value(s)" at the
  `memcmp`/comparison, with `--track-origins=yes` pointing at the `malloc`. That's why this exercise is Valgrind-graded.
- `tally_new` isn't tested with a huge count: ASan's `calloc` reports `calloc-overflow` and aborts rather than returning NULL.
