# M03 e05: Trusting the header `[BUG]`

## Goal

`records.c` parses fixed-size records from a byte buffer (the contents of a small binary
file). The header is a signed 32-bit `count`. The parser allocates `count + 1` records (one
sentinel) and copies records until it has `count` of them or the input runs out. `main.c`
holds four test inputs as byte arrays: `good` (a normal file), `big` (an absurd count),
`neg` (a negative count), and `shrt` (truncated). The harness parses all four. On the plain
build every check passes. Only the ASan build shows that `neg` writes past a 1-byte block.

Fix the validation, then write `ANSWER.md`.

## Constraints

- After the fix, `records_parse` returns NULL for any count with
  `!(count >= 0 && count <= RECORDS_MAX)`, and for input shorter than its header claims.
  `RECORDS_MAX` is in `records.h`.
- `ANSWER.md`: for `neg`'s count, show the value of `count + 1`, the byte size passed
  to `malloc`, and why the copy loop still wrote past the block. Say which existing check
  looked like it handled negative counts but didn't.
- Exactly one defect was planted, and it may span two lines. Keep your fix local to it.
- No leaks on any error path, including short input after a successful allocation.

## How to verify

- `make check`: all four inputs handled, with ASan and Valgrind clean.
