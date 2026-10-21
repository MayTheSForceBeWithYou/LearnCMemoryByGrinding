# m12-e07-uninit-read `[BUG]`

## Goal

`histogram` counts values into bins. The plain build passes every check; the ASan build
fails the checks **without printing any ASan report**.

Find and fix the bug. Write `ANSWER.md`: what was wrong, the mechanism (what happened in memory), and how the
tool showed it, quoting the lines of its report that mattered.
Explain both observations: why plain passes and why ASan fails with no report. This
exercise is graded by Valgrind: quote its report.

## Constraints

- Exactly one defect.
- Do not add a separate zeroing loop. Use the allocator that states your intent.

## Verify

`make check` passes; `make valgrind` is the stage that matters.
