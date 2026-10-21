# m12-e08-invalid-free `[BUG]`

## Goal

`count_words` counts words in a heap copy of a line. The plain build aborts.

Find and fix the bug. Write `ANSWER.md`: what was wrong, the mechanism (what happened in memory), and how the
tool showed it, quoting the lines of its report that mattered.
Include the address in ASan's report, and explain from the source which byte of the
block it is.

## Constraints

- Exactly one defect. Keep the heap copy (pretend the function later modifies it).

## Verify

`make check` passes.
