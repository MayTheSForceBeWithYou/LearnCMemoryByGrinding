# M05 e04 notes

**Bug 2 (API side), in `roster.c`.** `roster_add` stored `(char *)name`, the caller's pointer, instead of a copy. The header promises a copy.
`main` then frees `tmp` (legally, per the header), so every stored name dangles as soon as its loop iteration ends. The first read of
`roster_name(r, 0)` is a heap-use-after-free, with the free stack in `main` (`free(tmp)`) and the allocation in `strdup`. The `(char *)`
cast was the tell: casting away `const` to store a pointer is retaining a borrow.

**Bug 1 (caller side), in `main.c`.** `free((char *)first)` frees a block the roster owns. `roster_destroy` later frees it again, a
double free (and `roster_name(r, 4)` is fine, but any later read of slot 0 would be a use-after-free). Again a cast away from `const` is the giveaway.

With both bugs present, ASan reports the API-side UAF first, because it happens earlier. Fix it and the double free appears. That's normal:
ASan halts on the first error, so a file with two bugs shows them one at a time.

**Fixes:** `strdup` in `roster_add` (freeing the copy if the grow fails, so a failed add changes nothing), and delete the caller's `free`.
