# m10-e03-who-is-lost `[PREDICT]`

## Goal

`main.c` builds three lists of nodes with owned names and mishandles each one differently:

- **A**: the head pointer is overwritten;
- **B**: the destroy function frees every node but not the names;
- **C**: the list is destroyed correctly, except that one node was unlinked earlier and never freed.

**Before running anything**, write `PREDICTION.md`:

1. Total leaked blocks, split into direct and indirect, as LeakSanitizer will report them.
2. For A, B and C separately: which allocation lines appear and as which kind.
3. Valgrind's "definitely lost" and "indirectly lost" block counts for the same program.
   Same split as LeakSanitizer, or not? Why?

Then run `make asan` and `make valgrind`, and add a paragraph on anything you missed.

## Constraints

Prediction first, as written. Corrections go below it.

## Verify

The program builds. The tool output is the answer key.
