# m11-e02-three-stacks `[REPORT]`

## Goal

`capture.txt` is a real ASan report from this exact `main.c`. Using the report, write `ANSWER.md`:

1. For each of the three stacks (accessed at, freed by, previously allocated by), give the
   source line in `main.c` that matters and what happened there.
2. The freed-by and allocated-by stacks both end in the same line of `line_set`. Explain
   how one line can both free and allocate the block.
3. Which variable is the dangling pointer, and the smallest change to `main` that keeps the
   check's intent ("the first contents started with 's'") without it.

Then fix `main.c`.

## Constraints

- Answers 1-3 come from the report and the source, before running anything.
- Do not change `line_set` or `line_clear`. Their contracts are correct.
- The plain build passes this check by luck. Explain in one sentence why that is plausible.

## Verify

`make check` passes.
