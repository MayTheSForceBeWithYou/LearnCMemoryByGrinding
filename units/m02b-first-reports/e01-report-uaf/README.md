# M02b e01: Read the use-after-free report `[REPORT]`

## Goal

This folder ships `inventory.c`, `main.c`, and `capture.txt`. `capture.txt` is a real ASan
report from running the ASan build of exactly these files on the course machine.
**Before you build or run anything**, answer in `ANSWER.md`, from the report and the
source only:

1. The bug class, and whether the bad access is a read or a write, and of what size.
2. The file and line of the bad access (the first frame in course code).
3. The block involved: its size, the function that allocated it, and which element or
   field the access touched (work it out from "N bytes inside of").
4. The `free` that ended the block's lifetime: file, line, and function.
5. In one or two sentences: why does the code reach the access after the free? Name the
   pointer variable that is stale.

Then run `make asan` and confirm. Addresses, thread IDs, and the PID will differ on your
run, but the stacks and offsets shouldn't. Finally, fix the bug so `make check` is clean.

## Constraints

- No running, no gdb, and no added prints until the five answers are written.
- The fix must keep the program's behaviour. Deleting the access isn't a fix.

## How to verify

- `make asan` reproduces the report before your fix and is silent after it.
- `make check` is clean after the fix.
