# m14-e02-misaligned `[BUG]`

## Goal

`main.c` has a small arena whose `arena_alloc` promises memory "suitably aligned for any
object type". The plain build and Valgrind are happy. The sanitizer build is not.

Find and fix the bug. Write `ANSWER.md`: the UBSan message, the exact address offset from
`base` that it complains about (derive it from the source), why x86-64 ran the plain build
correctly anyway, and why neither ASan nor Memcheck reported anything.

## Constraints

- Exactly one defect, in `arena_alloc`. `main` and `Item` are correct.
- Keep the overflow safety of the fit test.

## Verify

`make check` passes.
