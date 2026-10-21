# m12-e03-uaf `[BUG]`

## Goal

A tiny settings store keeps owned copies of values. `main` remembers the old theme, sets a
new one, and checks both. The plain build fails a check, and ASan says more.

Find and fix the bug. Write `ANSWER.md`: what was wrong, the mechanism (what happened in memory), and how the
tool showed it, quoting the lines of its report that mattered.

## Constraints

- Exactly one defect. `settings_set`'s contract (old value freed on replace) is correct and stays.
- Explain in `ANSWER.md` why the plain build printed a wrong value instead of crashing.

## Verify

`make check` passes.

<details><summary>Hint</summary>

Read the borrowing rule in the comment above `settings_get`.
</details>
