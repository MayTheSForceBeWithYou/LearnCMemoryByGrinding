# m12-e07-uninit-read: notes

**Bug.** `bins = malloc(...)`, then `bins[k]++` on bytes never written.

**Mechanism.** Memcheck reports a conditional jump depending on uninitialised values at
`course_check_ (course.h:27)`, called from each `check` in `main`. That is four reports,
one per check, each with the origin at `histogram`'s `malloc`. The undefined
counters were *copied* and *incremented* silently. Only the comparison is reported.
Plain build: the block came from the top chunk, which is fresh zeroed kernel memory, so the
counts happen to be right. ASan build: ASan fills new blocks with `0xbe`, so each counter
starts at `0xbebebebebebebebe` and every check fails. ASan reported nothing because it does
not track initialisation.

**Fix.** `calloc(nbins, sizeof *bins)`. It also does the `n * size` overflow check for you.
