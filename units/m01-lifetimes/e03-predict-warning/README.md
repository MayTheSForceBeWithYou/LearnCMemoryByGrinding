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
3. Does your answer to question 2 change when the same file is built with optimization
   turned on? The plain build passes no `-O` flag, so gcc compiles at `-O0`: no
   optimization, and only the analysis needed to emit code. At `-O2` gcc runs its
   optimization passes, which track where values flow between statements, and several
   diagnostics (including some of the lifetime warnings) are produced by those passes. So
   a problem gcc cannot see at `-O0` may be reported at `-O2`, and the option named in
   brackets may differ. For each function, answer `no` if the `-O2` build gives the same
   result as the plain build. Otherwise, name what changed: the warning appeared, it
   disappeared, or it switched to a different option. This question is about the
   compiler's diagnostics, not the program's output; `main` never calls the invalid
   functions, so the run itself behaves the same either way.

Then build and compare.

## Constraints

- Predictions go in `ANSWER.md` *before* you run `make`. Record misses honestly, and for
  each one write the reason the compiler could or could not see the problem.
- Do not edit `main.c`. For question 3, run `make clean run EXTRA_CFLAGS=-O2`.

## How to verify

- `make run` shows the warnings for the plain build. The program itself only calls the
  valid functions, so it exits 0.
- `make check` passes as shipped.
