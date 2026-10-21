# M02 e05 notes

- One `malloc`, then `memcpy(d, src, n * sizeof *d)`. Size both with the *destination's* element type.
- `n == 0`: `memcpy(d, NULL, 0)` is technically undefined in C17 (null pointer arguments are invalid even for zero
  length. C2y relaxes this), and UBSan's `nonnull-attribute` check can flag it. Returning early avoids the question.
- The contract comment is the point: the source is *borrowed*, so the caller may free it the instant we return.
  The common bug is returning `(int *)src`, a "copy" that is really an alias. The harness's free-then-read check catches that under ASan.
