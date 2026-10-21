# m08-e03-lost-table `[PREDICT]`

## Goal

`main.c` builds three structures and loses each one in a different way:

- **A**: a jagged table of row strings whose only pointer goes out of scope;
- **B**: a flat `w*h` grid whose pointer is overwritten;
- **C**: a jagged table whose table block is freed, while its rows are not.

**Before running anything**, write `PREDICTION.md` answering:

1. How many leaked blocks LeakSanitizer reports in total, and how many of those are
   *direct* and how many *indirect*.
2. For each of A, B and C, which allocation line(s) appear in the report, and whether as direct or indirect.
3. Which of A, B and C would vanish from the report if you added one single `free` call, and which call that is.
4. The byte count of every record and of the `SUMMARY` line. Sizes and string lengths are in the
   source. Pointers are 8 bytes, and remember the NUL.

Then run `make asan` and compare. Under the prediction, add a short paragraph on anything you
got wrong and why.

## Constraints

- Do not edit `main.c` before writing your prediction. Afterwards, edit freely to test ideas.
- Commit (or keep) the prediction as you first wrote it. Corrections go below it, not over it.

## Verify

There is no pass/fail harness for the prediction itself. The program builds and its leak
report is the answer. Use `make asan` for LeakSanitizer and `make valgrind` for memcheck's
view of the same leaks ("definitely" vs "indirectly lost").
