# m12-e04-heap-overflow `[BUG]`

## Goal

`join` concatenates words with a separator. The plain build prints the right string; ASan disagrees.

Find and fix the bug. Write `ANSWER.md`: what was wrong, the mechanism (what happened in memory), and how the
tool showed it, quoting the lines of its report that mattered.
State the exact number of bytes written past the block, derived from the source, and show
where the report confirms the block size. Then explain why the plain build passed.

## Constraints

- Exactly one defect. Keep a single allocation of the exact size.
- The size computation must still be correct for `n == 0` and `n == 1`.

## Verify

`make check` passes.
