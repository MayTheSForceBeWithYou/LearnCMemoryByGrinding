# m12-e09-under-allocation `[BUG]`

## Goal

`make_points` builds `n` points on a line. The plain build aborts inside the harness, in a
place that has nothing to do with points.

Find and fix the bug. Write `ANSWER.md`: what was wrong, the mechanism (what happened in memory), and how the
tool showed it, quoting the lines of its report that mattered.
Explain why the plain build's abort message names `malloc`, and which call to `malloc` it was.

## Constraints

- Exactly one defect, one token.

## Verify

`make check` passes.

<details><summary>Hint</summary>

Compute, from the ASan report, how many bytes were requested and how many the loop writes.
</details>
