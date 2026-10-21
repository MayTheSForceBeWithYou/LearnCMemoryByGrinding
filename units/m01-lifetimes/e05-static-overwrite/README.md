# M01 e05: Why does it print the same name twice? `[EXPLAIN]`

## Goal

`main.c` is a tiny inventory report. It uses `item_name(int id)`, which returns a pointer
to a `static char[32]`, and prints lines such as

```c
printf("swap %s <-> %s\n", item_name(a), item_name(b));
```

It also keeps an array of `const char *` names gathered in a loop and prints them
afterwards. The output is wrong in two different ways. The program is valid C with no
undefined behaviour, and ASan and Valgrind are both clean.

Write `ANSWER.md`. There is no code to change.

## What ANSWER.md must contain

1. For the `printf` line: what the two `%s` arguments point at, which text both of them
   print, and what "unspecified order of evaluation" changes about *which* text that is.
2. For the gathered-names loop: how many distinct `char` objects exist, how many pointers
   point at each, and what every element of the array prints.
3. Why no sanitizer can find this.
4. The smallest change to `item_name`'s *interface* (not its body alone) that makes both
   uses correct, and which lifetime strategy from the lesson it uses.

## Constraints

- Answer from reading the source first. Then run `make run` to confirm.
- Optional: apply your interface change on a copy and confirm the output. The graded
  artifact is `ANSWER.md`.

## How to verify

- `make check` passes on the shipped program (its checks only confirm it runs). The
  correctness of your explanation is checked against the solution's NOTES.md.
