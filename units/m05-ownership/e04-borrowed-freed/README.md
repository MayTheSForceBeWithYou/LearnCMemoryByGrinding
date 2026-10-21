# M05 e04: Broken promises `[BUG]`

## Goal

`roster.c` stores players by name. `main.c` uses it. The program builds. It already misbehaves on the plain
build: one check fails, and then glibc aborts on a double free. ASan and Valgrind tell you
*which* blocks are involved and where they were freed.

There are **two** planted ownership bugs, one on each side of the API:

- one where a **caller frees something it only borrowed**;
- one where the **API keeps a borrow** of an argument that the caller is entitled to free
  right after the call.

Fix both, and write `ANSWER.md`.

## Constraints

- Fix each bug on the side whose contract is wrong. To decide which side that is, read the
  header comments in `roster.h`. They are correct, and the code disagrees with them in two places.
- `ANSWER.md`: for each bug, the block involved, its owner according to the header, the call that
  freed it, and the later access that touched it.
- Don't change `roster.h`.

## How to verify

- `make check`: checks, ASan, and Valgrind clean.
