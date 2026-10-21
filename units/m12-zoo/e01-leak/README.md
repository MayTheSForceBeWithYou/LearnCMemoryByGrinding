# m12-e01-leak `[BUG]`

## Goal

`count_equal` normalizes each word into a temporary heap string and compares it with the
normalized target. The checks pass, but the program is not clean.

Find and fix the leak. Write `ANSWER.md`: what was wrong, the mechanism (what happened in memory), and how the
tool showed it, quoting the lines of its report that mattered.

## Constraints

- Exactly one defect. Fix it with the smallest change that keeps `normalize` returning an owned string.
- Your `ANSWER.md` must say *which inputs* leak and why the plain build cannot tell you.

## Verify

`make check` passes: checks, ASan/LSan with no report, Valgrind clean.

<details><summary>Hint</summary>

The LSan summary counts allocations. Compare that number with the input array.
</details>
