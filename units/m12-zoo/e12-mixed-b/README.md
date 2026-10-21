# m12-e12-mixed-b `[BUG]`

## Goal

A jagged grid of `int` rows. **Three** bugs, of unspecified classes. The plain build aborts.

Find and fix all three. Write `ANSWER.md` with one section per bug: what was wrong, the
mechanism, and which tool showed it. One of them can be seen by only one of the course's tools: say which, and why.

## Constraints

- Exactly three defects.
- `grid_init` must still free everything it allocated when it fails.

## Verify

`make check` passes.

<details><summary>Hint</summary>

After ASan is clean, you are not done.
</details>
