# M06 e04: Predict the sweep `[PREDICT]` `[FAULT]`

## Goal

`main.c` is complete and correct. It builds a `struct cache` (struct, name copy, slot array) twice,
and each cache also tries an **optional** speed-up table. If that table can't be allocated, the
cache works without it, and the program still passes.

Before running anything, predict in `ANSWER.md`:

1. N, the total number of allocation calls a normal run makes (what `FA_COUNT=1` prints). Count
   only calls in `main.c`. Say why libc's own allocations aren't counted.
2. For every `FAIL_AT` value from 1 to N: does the run exit 0 (checks pass) or 3 (`EXIT_OOM`)?
3. The summary line `make fault` prints (`fault-sweep: N=.. pass=.. oom=.. fail=0`).

Then run `make fault` and compare.

## Constraints

- Don't edit `main.c`.
- For every prediction you got wrong, say which allocation you miscounted or misclassified.

## How to verify

- `make fault` prints the sweep summary. `make check` passes as shipped.
