# M02 e03: The stale copy `[BUG]`

## Goal

`main.c` keeps a heap array of scores and a `struct board` that holds a second pointer
to the same array "for display". A `reset_scores` function frees the array and allocates a
fresh one, and then the board prints. The plain build passes every check: the stale pointer
happens to still read the right values. The ASan build stops with a `heap-use-after-free` report.

Fix the bug, then write `ANSWER.md`.

## Constraints

- After the fix, the board must show the *new* scores. Making the board stop reading the
  array isn't a fix.
- `ANSWER.md` must say which block the board's pointer referred to, which call ended that
  block's lifetime, and why the plain build still passed its checks. Also say
  why `scores = NULL` after the `free` (if you add it) wouldn't fix the bug by itself.
- Exactly one defect was planted. Don't restructure the program beyond what the fix needs.

## How to verify

- `make check`: checks pass, ASan is silent, Valgrind is clean.
- Before fixing, run `make asan` once and keep the report. You'll read one like it in M02b.
