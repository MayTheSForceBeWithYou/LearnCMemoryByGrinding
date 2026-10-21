# M07 e02: One byte short `[BUG]`

## Goal

`label.c` builds display labels such as `"[Lv 7] Ada!"` from a name and a level, and a
`label_upper` variant. The plain build passes every check. ASan reports a
`heap-buffer-overflow` of size 1.

Fix it, then write `ANSWER.md`.

## Constraints

- `ANSWER.md`: the size of the block that was allocated, the length of the string written into it,
  the exact index of the bad write, why the plain build passes anyway (think about what `malloc`
  actually hands out), and whether `make valgrind` catches it, and why.
- Exactly one sizing defect was planted. The fix is to the size computation, not a larger fixed buffer.

## How to verify

- `make asan` reports the overflow before the fix. `make check` is clean after it.
