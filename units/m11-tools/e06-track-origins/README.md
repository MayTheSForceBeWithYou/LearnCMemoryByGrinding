# m11-e06-track-origins `[REPORT]`

## Goal

`capture.txt` is a real memcheck report (`make valgrind`, which uses `--track-origins=yes`)
from this exact `main.c`. Notice that the checks *passed* in the same run. Write `ANSWER.md`:

1. Which value is uninitialised, where it is used (first stack) and where it was born (origin stack).
2. Why memcheck reports at line 24 and not at line 14, although line 14 is where the mistake is.
3. What `make asan` prints for this program and why. Predict, then run it.
4. Why the plain build's check passes, and why that is no evidence the program is correct.

Then fix `box_new` so that `make check` passes.

## Constraints

- Fix the constructor, not `box_area`. Every field of a `Box` must be defined after `box_new` returns.

## Verify

`make check` passes. Valgrind grades this one: the ASan stage cannot see the bug.
