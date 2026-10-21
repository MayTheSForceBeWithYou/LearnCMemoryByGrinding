# m11-e04-direct-indirect `[PREDICT]`

## Goal

`main.c` loses two structures: **A**, a three-node list of owned names whose head pointer is
dropped, and **B**, a table of two `int` blocks where only the table is freed.

**Before running anything**, write `PREDICTION.md`:

1. Every leak record LeakSanitizer will print: direct or indirect, byte count, object count,
   and the `main.c` line of the allocation. Records with the same stack are merged.
2. The `SUMMARY` line: total bytes and total allocations.
3. For B: why freeing the table *turns its rows into direct leaks* rather than indirect ones.

Then run `make asan` and compare. Write corrections below your prediction, not over it.

## Constraints

- Sizes are part of the prediction. `sizeof(Node)` on x86-64 is 16. Work out string sizes yourself.
- Do not edit `main.c` before predicting.

## Verify

There is no pass/fail for a prediction. `make asan` gives the answer and exits non-zero
because of the leaks, which is expected here.
