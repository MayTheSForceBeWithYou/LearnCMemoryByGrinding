# m08-e04-flat-grid: notes

**Why it is correct.** The overflow test `w > SIZE_MAX / h` runs before any multiplication, and `h != 0`
is already known. The struct is freed if the cell allocation fails. One `static` helper, `cell()`, owns
both the bounds check and the `y * w + x` formula, so the accessors cannot disagree about the layout.
Because `x < w` and `y < h`, the index is less than `w * h`, which was shown not to overflow.

**Alternatives.** A flexible array member (`char cells[]`) puts the struct and cells in one allocation:
`malloc(sizeof *g + w * h)`, with its own overflow check on the sum. `calloc(w, h)` is not a good substitute
here: it zeroes bytes that `memset` immediately overwrites, and it moves the overflow test into the library
instead of rejecting the size before any allocation call, which is what the README asks for. Keep the explicit test.

**The usual mistake.** Writing the overflow check as `w * h < w` or `w * h > SIZE_MAX`. The first is not a
correct overflow test for multiplication, and the second is always false. The harness's
`(SIZE_MAX / 2 + 2) x 2` cases wrap to 2 bytes, so a missing or wrong test makes `malloc(2)` succeed and the
`== NULL` check fails on every build, not only when ASan notices a later out-of-bounds write.
