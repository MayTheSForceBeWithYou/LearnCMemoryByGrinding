# M04 e04: Predict `realloc` `[PREDICT]` `[NO-ASAN]`

## Goal

`main.c` runs six `realloc` scenarios selected by `argv[1]`:

1. `realloc(NULL, 32)`
2. grow a 32-byte block to 4096 bytes, and read a byte from the grown tail through Valgrind
3. shrink a 4096-byte block to 16, then read the byte at offset 15 (in bounds) and at offset 16 (one past the new end, on purpose)
4. `realloc(p, 0)` on glibc, then `free(p)` on the original pointer
5. grow by 1 byte, 10000 times (with one small unrelated allocation after each grow, as in a real program), and count how many times the address changed
6. the same as 5, but doubling

For each, predict in `ANSWER.md` what you expect to see from the plain build, from
`make valgrind ARGS=N`, and (for 3 and 4) from `make asan ARGS=N`: a clean run, a report
(name the class), or a glibc abort. For 5 and 6, give a rough count and the reason for it.

## Constraints

- Scenarios 3 and 4 contain deliberate undefined behaviour. They exist so you see what the tools
  say. This is why the exercise is `[NO-ASAN]` for its default `make check`, which runs only
  1, 5, and 6.
- For scenario 4, explain why the course forbids this call, using what you observed.

## How to verify

- `make check` passes as shipped (scenarios 1, 5, 6).
- Run the rest by hand and record the observed results next to your predictions.

