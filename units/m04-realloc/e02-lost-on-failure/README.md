# M04 e02: Lost on failure `[BUG]` `[FAULT]`

## Goal

`strbuf.c` implements a string builder with `sb_append(struct strbuf *, const char *)`.
It passes `make check`'s normal checks, ASan, and Valgrind, because without injected
failures the growth code never takes its failure path. It fails `make fault`: as shipped,
**every** run of the sweep fails (`N=6 pass=0 oom=0 fail=6`). At `FAIL_AT=1`, UBSan
reports a null pointer passed to a library function. At `FAIL_AT=2` through `6`, ASan
reports a SEGV writing near address 0. No leak report and no heap overflow appear yet.

The two defects interact. While the first is present, the program dies on a NULL pointer
before the second can do any damage. Once you fix the first, the sweep changes shape and
the second defect shows up as a heap-buffer-overflow.

There are **two** planted defects in the growth code, both about the failure path.
Find both, fix both, and write `ANSWER.md`.

## Constraints

- `ANSWER.md` names, for each defect: the `FAIL_AT` value (or values) that exposes it, what
  the old block's state is at that moment, and the exact consequence: what the sanitizer
  reported, and what would have happened without it (which block leaks, or which write
  overflows which block, and by how much).
- On allocation failure, `sb_append` returns -1 and leaves the builder exactly as it was
  before the call. `main` then frees the builder and exits with `EXIT_OOM`.
- Change only what the two fixes need.

## How to verify

- `make fault` is the grading run: every `FAIL_AT` must exit 0 or 3, with no sanitizer report.
- `make check` (which includes the sweep, since this exercise is tagged `[FAULT]`) is clean.
