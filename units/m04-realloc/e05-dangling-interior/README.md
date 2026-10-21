# M04 e05: Pointers into a moving buffer `[BUG]`

## Goal

`lexer.c` reads input chunk by chunk into one growable buffer, and records each token as a
`const char *start` plus a `size_t len` pointing into that buffer. When the input is
small, everything works. With the larger test input, ASan reports a heap-use-after-free
in `token_print`.

Fix the design, then write `ANSWER.md`.

## Constraints

- Tokens must keep working however many times the buffer grows. Pre-allocating a huge
  buffer isn't a fix.
- The public API in `lexer.h` may change in how a token refers to its text, but
  `token_text(const struct lexer *, size_t i, size_t *len)` must still return a pointer the
  caller can read **until the next call that may grow the buffer**. Document that rule in the header.
- `ANSWER.md`: which block the stale `start` pointed into, which call freed it, and why the
  small input didn't trigger the bug.
- Exactly one defect was planted, conceptually: storing pointers across a reallocation.

## How to verify

- `make check`: checks with both inputs, ASan, and Valgrind clean.
