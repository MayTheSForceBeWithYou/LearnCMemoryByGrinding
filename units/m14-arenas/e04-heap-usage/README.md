# m14-e04-heap-usage `[PREDICT]`

## Goal

`main.c` builds the same 1000-node list two ways: one `malloc` per node, or one block for
all nodes. **Before running anything**, write `PREDICTION.md` with, for each mode
(`list`, `arena`):

1. The `total heap usage` line Valgrind prints: allocs, frees, bytes allocated.
2. How much heap the program actually occupies for the nodes (chunk sizes, not request sizes),
   using what M12 and LESSON.md say about glibc's chunks.
3. Which figure in Valgrind's summary reflects the difference, if any.

Then run `make usage` (Valgrind on both modes, keeping only the summary lines) and compare.

## Constraints

- `sizeof(Node)` is 16 on x86-64.
- Something besides the nodes allocates once. Account for it.

## Verify

No pass/fail. Compare with `solutions/m14-arenas/e04-heap-usage/NOTES.md` after writing
your prediction.
