# M02 e06: Trace the blocks `[TRACE]`

## Goal

`main.c` performs a short sequence of `malloc`, pointer copies, `free`, and
`p = NULL` assignments on three pointer variables and two heap blocks. Five lines are
marked `/* TRACE 1 */` to `/* TRACE 5 */`. At each mark, draw:

- each pointer variable, and whether it points to a **live block** (name it B1 or B2),
  a **freed block** (dangling), or is NULL;
- each heap block, whether it is live or freed, and how many pointer variables currently
  hold its address;
- whether a block is **leaked** at that point (live, with no pointer holding its address).

```
TRACE _
  p ---> [ B1 | live | 24 B ]
  q ---> (dangling: B1 freed)
  r = NULL
  leaked: none
```

## Constraints

- Draw first, then run the program. Its output prints the *addresses* (as `%p`) at each mark,
  which is enough to check which variables alias each other. It can't tell you whether a
  block is alive, so that part you must reason out.
- The program is valid C. It never dereferences a dangling pointer, and it records each
  address as a `uintptr_t` *before* any free, so it never even reads a dangling pointer's value.
- One block becomes unreachable from `p`, `q`, and `r` partway through. Say at which mark
  that first happens. To keep the course's "done means clean" rule, the program secretly
  keeps a backup pointer to that block in a `static` and frees it at the end. A real
  program would have no such backup, and the block would be leaked. Ignore the backup
  when you draw.

## How to verify

- `make run` prints the addresses. `make check` passes as shipped. The drawing is the exercise.
