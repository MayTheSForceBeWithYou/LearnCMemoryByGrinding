# m12-e05-heap-underflow `[BUG]`

## Goal

`rtrim` removes trailing spaces in place. Three of the four inputs behave; the plain build
passes all checks.

Find and fix the bug. Write `ANSWER.md`: what was wrong, the mechanism (what happened in memory), and how the
tool showed it, quoting the lines of its report that mattered.
Explain why the report says `before` and not `after`, and why the plain build did not
notice even though the loop ran off the start of the block.

## Constraints

- Exactly one defect, in `rtrim`. `main` is correct.
- Your fix must also be correct for the empty string `""`, which the harness does not test.

## Verify

`make check` passes.
