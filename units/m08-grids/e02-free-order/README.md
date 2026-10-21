# m08-e02-free-order `[BUG]`

## Goal

`main.c` builds a jagged grid of labelled rows, uses it, and tears it down. The program
builds, and on a plain run it usually prints the right thing and exits 0. It is still wrong.

Find the defect, fix it with the smallest correct change, and write `ANSWER.md`. Read every
line the build prints, not just the program's output.

## Constraints

- There is exactly one bug. Do not restructure the program.
- `ANSWER.md` (a few sentences): what the code did, the mechanism (why it is undefined
  behaviour and why the plain run can still look fine), and which line of the tool report
  pointed you to it.

## A note on compiler warnings

gcc 16's `-Wuse-after-free` sometimes catches this kind of bug at compile time, but not reliably.
A build without that warning proves nothing. The runtime tools (`make asan`, `make valgrind`) are the
real check.

## Verify

```bash
make run        # see it "work"
make asan       # see why it doesn't
make check      # after your fix: everything clean
```

After the fix, the harness prints `m08-e02-free-order: all checks passed`, and ASan and
Valgrind are silent.
