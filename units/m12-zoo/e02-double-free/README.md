# m12-e02-double-free `[BUG]`

## Goal

`doc_copy` is documented to make an independent copy, and the program destroys both
documents. It aborts in the plain build.

Find and fix the bug. Write `ANSWER.md`: what was wrong, the mechanism (what happened in memory), and how the
tool showed it, quoting the lines of its report that mattered.
Include the glibc message from `make run` and the ASan report header, and explain why both
point at the second `doc_fini`, although that line is correct.

## Constraints

- Exactly one defect. `doc_fini` and `main` are correct and must not change.
- The copy must still report allocation failure through its return value, and leak nothing when it fails.

## Verify

`make check` passes.
