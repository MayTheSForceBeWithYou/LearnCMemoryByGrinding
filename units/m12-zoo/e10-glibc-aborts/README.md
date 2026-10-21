# m12-e10-glibc-aborts `[BUG]` `[NO-ASAN]`

## Goal

**`[NO-ASAN]`: this exercise is about glibc's own checks.** Use `make run` and `make valgrind`.
`make asan` also works and will show you the same bugs, but the point is to read glibc's messages.
"Done" means `make check` passes (it skips ASan for this tag) and `make valgrind` is clean.

`main.c` has **two** bugs, one in Part A and one in Part B. Each makes glibc abort with a
different message. You will only see the second after fixing the first.

Fix both. Write `ANSWER.md`: what was wrong, the mechanism (what happened in memory), and how the
tool showed it, quoting the lines of its report that mattered.
For each: the exact glibc message, which glibc check produced it, and why the line where it
aborted is not the line with the bug.

## Constraints

- Exactly two defects. `main` is correct, including calling `reader_close` twice: the
  comment above `reader_close` is the contract.
- Do not add sanitizer flags to the Makefile.

## Verify

`make check` and `make valgrind` pass.
