# M03 e04: Predict the conversions `[PREDICT]`

## Goal

`main.c` prints ten expressions that mix `int`, `unsigned`, `uint32_t`, and `size_t`, such
as `(size_t)-1`, `-1 * sizeof(int)`, `(int)3000000000u`, `n - 1` for `size_t n = 0`, and
`w * h * 4` with `uint32_t` operands. Each is printed with `%zu` or `%d` after an explicit
conversion, so the program is free of UB.

For each line, write in `ANSWER.md` before running:

1. The type the arithmetic is performed in (after the integer promotions and the usual
   arithmetic conversions).
2. The exact printed value. You can write 2^64 - k, and so on, then convert to decimal.
3. Whether the expression would be a bug if it were passed to `malloc`, and why.

Then `make run` and compare.

## Constraints

- Assume x86-64 Linux: `int` is 32 bits, and `size_t` and `long` are 64.
- One line involves an implementation-defined conversion (signed from out-of-range). Say which,
  and what gcc documents it does.
- Don't edit `main.c`.

## How to verify

- `make check` passes as shipped. It also enables `-Wconversion` for this one exercise. Read the
  warnings, and add to `ANSWER.md` which of your ten lines the compiler flagged.
