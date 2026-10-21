# M02b e03: Predict the Valgrind leak summary `[PREDICT]`

## Goal

`main.c` makes a fixed, deterministic set of allocations (no loops that depend on input),
and frees some of them. Some blocks are lost directly, some are lost only because their
owner was lost, and one is still pointed to by a global at exit.

Before running Valgrind, predict in `ANSWER.md`:

1. The number of allocations and frees that `main.c` itself makes. Valgrind's "total heap
   usage" line counts more: the first `printf` makes stdio allocate a buffer for `stdout`
   (4 KiB on glibc) that is never freed. Account for the difference after you run it.
2. **definitely lost**: bytes and blocks.
3. **indirectly lost**: bytes and blocks.
4. Anything else you expect to appear in the leak summary (one category in this program
   is neither definitely nor indirectly lost). Name it if you can, and say why it is
   different. M11 covers it in depth.
5. Whether `make valgrind` (which uses `--errors-for-leak-kinds=definite,indirect,possible
   --error-exitcode=1`) exits 0 or 1.

Then run `make valgrind` and record what you got next to each prediction.

## Constraints

- Assume x86-64 glibc: pointers and `size_t` are 8 bytes and `int` is 4. Count requested
  bytes. Valgrind reports what you asked for, not the allocator's rounded-up size.
- If Valgrind refuses to start, see the lesson section "Valgrind on Arch".
- After recording, fix the program so `make check` passes. Decide whether the
  global-pointer block needs freeing to pass, and whether it *should* be freed anyway.
  Write your reasoning in `ANSWER.md`.

## How to verify

- `make valgrind` shows the summary you predicted against.
- `make check` is clean after your fix.
