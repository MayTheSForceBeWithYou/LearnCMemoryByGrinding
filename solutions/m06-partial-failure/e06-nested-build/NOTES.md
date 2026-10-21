# M06 e06 notes

- `calloc` for the pointer array makes "the cards copied so far" and "all n slots" the same set for `free`, because uncopied slots are NULL.
  Setting `d->n = n` *before* the copy loop is what lets `deck_destroy` serve as the cleanup.
- The alternative without `calloc`: track `i` and free `cards[0..i-1]` on failure (`k < i`), then the array, title, and struct. That's
  correct, but it's a second teardown routine that can drift out of sync with `deck_destroy`.
- `n == 0`: no array allocation at all. `calloc(0, ...)` might return NULL and look like failure. `cards` may be NULL, as documented.
- Sweep: 13 calls for the 10-card deck (struct, title, array, 10 strings) plus 2 for the empty deck, so N = 15, and all of them exit 3.
