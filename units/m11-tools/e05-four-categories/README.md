# m11-e05-four-categories `[PREDICT]`

## Goal

`main.c` makes four allocations (marked 1-4) plus one more, then leaves each in a different
state at exit. **Before running anything**, write `PREDICTION.md`:

1. For each of the five blocks, its memcheck category (definitely lost, indirectly lost,
   possibly lost, still reachable) and size.
2. The four lines of the `LEAK SUMMARY`.
3. Whether `make valgrind` (the course flags: errors for definite, indirect and possible)
   exits 0 or 1, and how many errors its `ERROR SUMMARY` counts.
4. Which of the five blocks LeakSanitizer would report, and which it would not.

Then run `make valgrind-all` (memcheck with `--show-leak-kinds=all`), then `make valgrind` and `make asan`, and compare.

## Constraints

- Do not edit `main.c` before predicting.
- `sizeof(Box)` is 8 on x86-64.

## Verify

No pass/fail; the three runs are the answer.

<details><summary>Hint</summary>

The categories depend on what points to a block's *start*, what points into its *middle*,
and whether the pointer lives in a root (global, stack, register) or in another lost block.
</details>
