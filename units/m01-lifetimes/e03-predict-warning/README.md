# M01 e03: What does the compiler catch? `[PREDICT]`

## Goal

`main.c` contains six small functions, `f1` to `f6`. Each returns a pointer. One
returns the address of an automatic object directly, two launder it (through a second
local pointer, or a struct field), and the rest return addresses that are perfectly valid
(a string literal, a static, a pointer parameter).

Before compiling, write in `ANSWER.md`, for each function:

1. Is the returned pointer valid in the caller? (yes or no, and the storage duration of the
   pointed-to object)
2. Will `gcc -std=c17 -Wall -Wextra -Wpedantic -g` (the plain build) warn about it? (yes or no, and if
   yes, the warning option named in brackets)
3. Does the answer change at `-O2`?

Then build and compare.

## Constraints

- Predictions go in `ANSWER.md` *before* you run `make`. Record misses honestly, and for
  each one write the reason the compiler could or could not see the problem.
- Do not edit `main.c`. For question 3, run `make clean run EXTRA_CFLAGS=-O2`.

## How to verify

- `make run` shows the warnings for the plain build. The program itself only calls the
  valid functions, so it exits 0.
- `make check` passes as shipped.
