# m12-e06-off-by-one-nul `[BUG]`

## Goal

`prefix` returns a heap copy of the first `n` bytes of a string. Every run of the plain
build and every check passes.

Find and fix the bug. Write `ANSWER.md`: what was wrong, the mechanism (what happened in memory), and how the
tool showed it, quoting the lines of its report that mattered.
Explain why `make valgrind` alone would also have caught it, and why the plain build never will at this size.

## Constraints

- Exactly one defect. The function must keep returning a NUL-terminated string of exactly `n` characters.

## Verify

`make check` passes.
