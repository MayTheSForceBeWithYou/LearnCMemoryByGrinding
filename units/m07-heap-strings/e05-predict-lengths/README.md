# M07 e05: Lengths, sizes, and what was written `[PREDICT]`

## Goal

`main.c` prints twelve values: `sizeof` versus `strlen` for an array, a pointer, and a literal;
the length of `strndup` results for several `n`; `snprintf` return values for buffers that are
too small; and the bytes left in a buffer after a truncating `snprintf`.

Predict each value in `ANSWER.md`, then `make run` and compare.

## Constraints

- Assume x86-64 (pointers are 8 bytes).
- For each line you got wrong, write the rule you'd misremembered, in one sentence.
- Don't edit `main.c`.

## How to verify

- `make check` passes as shipped (the program frees everything it duplicates).
