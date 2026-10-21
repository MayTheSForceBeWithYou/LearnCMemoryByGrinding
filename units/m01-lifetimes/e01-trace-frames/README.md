# M01 e01: Trace the frames `[TRACE]`

## Goal

`main.c` is a complete program. It calls `outer`, which calls `inner`, and it touches one
`static` counter and one heap block along the way. Three points in the source are marked
`/* TRACE POINT A */`, `B`, and `C`. For each point, draw the state of memory on paper
(or in `ANSWER.md` as ASCII) using the template below.

## What to draw at each point

- Every live stack frame, innermost at the bottom, with each local's name and value.
  For pointers, draw an arrow to what they point at, or write `dangling` or `NULL`.
- The static data area: every object with static storage duration and its value.
- The heap: every live block, its size in bytes, and its contents if known.
- Anything that *was* alive at an earlier point and is now dead: cross it out, and say
  which closing brace or `free` ended it.

```
POINT _
STACK                         STATIC                HEAP
+-------------------+        +---------------+     +----------------+
| frame: ______     |        |               |     | block ___ B    |
|   name = value    |        |               |     |                |
+-------------------+        +---------------+     +----------------+
```

## Constraints

- Do the drawing **before** running anything. Then run the program and use its output
  (and `gdb`, if you like: `break` on the marked lines, `info locals`, `p &x`) to check
  your answer.
- Wherever a value is indeterminate (uninitialised or in a dead frame), write `?`. Do not
  guess a number.

## How to verify

- `make run` prints the values the program can legally observe at each point.
- `make check` must pass. The program is clean, so ASan and Valgrind report nothing.
  The exercise is the drawing.
- Compare with your drawing. Every disagreement is something to explain in one sentence
  in `ANSWER.md`.

<details><summary>Hint</summary>

The `static int calls` inside `inner` is not in `inner`'s frame, even though it is
declared inside `inner`. Draw it in the STATIC column at all three points. At point A,
inside `inner` just after `calls++`, it already holds its incremented value, and it existed
(zero-initialised) before `inner` was ever called.

</details>
