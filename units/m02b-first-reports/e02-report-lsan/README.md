# M02b e02: Read the leak report `[REPORT]`

## Goal

`main.c` builds a small `struct roster` that owns a heap array of `struct player`, each of
which owns a heap name buffer. `capture.txt` is the LeakSanitizer output captured from this
exact source at exit. It lists more than one leak record, including direct and indirect leaks.

From the report and the source only, fill in the table in `ANSWER.md`. Give one row per leak
record:

| record | direct or indirect | bytes and objects | allocation site (file:line) | what the block is | why it's unreachable |
|---|---|---|---|---|---|

Then answer: **what is the smallest set of missing `free` calls that removes every
record?** Predict how many records disappear after each `free` you add, in the order you'd add them.

## Constraints

- No running until the table is complete.
- Fix the leaks by adding frees in the right places. Don't restructure the data types.
  The final program must free in an order that never touches a freed block.

## How to verify

- `make asan` shows the same records before your fix. Byte counts and sites should match
  `capture.txt` exactly. Addresses don't appear in leak records except in stack frames.
- `make check` is clean after the fix (LSan and Valgrind both report zero leaks).
