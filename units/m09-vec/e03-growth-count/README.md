# m09-e03-growth-count `[PREDICT]`

## Goal

`main.c` pushes the same number of `int`s into two vectors. One grows by doubling from an initial
capacity, the other grows by exactly one element per push. Both are freed at the end. The source
states the element count and the initial capacity.

**Before running anything**, write `PREDICTION.md`:

1. How many calls to allocation functions each strategy makes (`realloc` acting as `malloc`
   on the first call counts).
2. The total that `FA_COUNT=1` will print for the whole program. Read `lib/failalloc.h` first for
   exactly which calls it counts.
3. Roughly how many `int` copies grow-by-one *could* cost in the worst case, as a formula and as a number.
   Then say why the actual run is likely much faster than that worst case.

Then run:

```bash
make build-fault                # the fault-injection build, without running a sweep
FA_COUNT=1 ./build/fault/prog   # prints "failalloc: N allocations" to stderr
```

and compare. Add a paragraph on any miss.

## Constraints

Prediction first, as written. Corrections go below it.

## Verify

The program builds and passes its own checks (`m09-e03-growth-count: all checks passed`).
The prediction is checked against the `FA_COUNT` line.

