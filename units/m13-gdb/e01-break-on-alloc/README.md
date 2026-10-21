# m13-e01-break-on-alloc `[TRACE]`

## Goal

`main.c` pushes 10 ints onto a growable `Vec`. Using gdb only (no `printf` added to the
source), write `TRACE.md` with:

1. A table with one row per `realloc` call: the old pointer, the requested size in bytes,
   the value `x` being pushed, and whether the returned block moved.
2. Every `free` call the program makes before `POINT B`, if any.
3. At `POINT B` (line 36): the value of `v`, the ten elements, and the output of
   `print v.data[v.len - 1]@2`. Explain the second number in that last output.

Start like this:

```
make build
gdb -q build/plain/prog
(gdb) break realloc
(gdb) break main.c:36
(gdb) run
```

At the `realloc` breakpoint, libc has no debug info. Read the arguments from registers
(LESSON.md, "What gdb needs"), and use `bt 2` to find the caller's `x`. Use `finish` to see the
returned pointer. Then turn the breakpoint into a logger with `commands` so you need only
one `run`.

## Constraints

- Do not edit `main.c`.
- Bonus: put your commands in a file `trace.gdb` that runs the whole trace with
  `gdb -q -batch -x trace.gdb build/plain/prog`.

## Verify

`make check` passes (the program is correct). Compare `TRACE.md` with
`solutions/m13-gdb/e01-break-on-alloc/capture.txt`, a real `gdb -batch` transcript.
