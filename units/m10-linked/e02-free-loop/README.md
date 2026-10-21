# m10-e02-free-loop `[BUG]`

## Goal

`main.c` builds a playlist as a linked list, prints it, and destroys it. On the course machine's
plain build it printed the right output and exited 0 on every run we tried. That's not a guarantee.
It contains one memory bug.

Find it, fix it with a minimal change, and write `ANSWER.md`.

## Constraints

- Exactly one bug. Don't restructure.
- `ANSWER.md`: the line, the mechanism, and why the plain build "works". Look at where `next` sits in
  `Track`. Then run the plain binary with `MALLOC_PERTURB_=165` (see `man 3 mallopt`), and again with
  `GLIBC_TUNABLES=glibc.malloc.tcache_count=0 MALLOC_PERTURB_=165`, and explain the difference.

## A note on compiler warnings

gcc 16's `-Wuse-after-free` sometimes catches this kind of bug at compile time, but not reliably.
A build without that warning proves nothing. The runtime tools (`make asan`, `make valgrind`) are the
real check.

## Verify

```bash
make run
make asan
make check      # after the fix
```

After the fix: `m10-e02-free-loop: all checks passed`, and ASan and Valgrind are silent.
