# M04 e03: Did it move? `[TRACE]`

## Goal

`main.c` builds a buffer, takes a pointer and an offset into it, and grows it twice:
first by a few bytes, then by a lot. After each grow, it prints whether `realloc` returned
the same address, and it reads through the offset only (never the old pointer).
Three points are marked `/* TRACE 1..3 */`.

For each mark, draw:

- every heap block that has existed so far (B1, B2, ...), with its size and whether it
  is live or freed;
- `buf.data`, `saved_ptr`, and `buf.data + saved_off`, and what each points at;
- which byte ranges of the live block hold defined data and which are the uninitialised
  grown tail. Write the ranges in C form, for example `i >= 0 && i < 12`.

Draw **both** possibilities at marks 2 and 3: in place and moved. Then run the program and
circle the one that happened.

## Constraints

- Drawings come first. The run tells you which branch you're in on this machine and allocator.
  It doesn't change what the code may assume.
- In `ANSWER.md`, say which of the program's later operations would be undefined if they
  used `saved_ptr` instead of the offset, in each branch.

## How to verify

- `make run` prints the move/no-move result for each grow. `make check` passes as shipped.
