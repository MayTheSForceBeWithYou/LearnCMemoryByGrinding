# m13-e04-vgdb `[BUG]` (graded by Valgrind)

## Goal

`items_new` builds an array of items and sets a `HEAVY` flag on the heavy ones.
`make run` and `make asan` both pass. `make valgrind` does not.

Find the bug **from inside Memcheck**, with vgdb. Terminal 1:

```
make build
valgrind --vgdb=yes --vgdb-error=0 ./build/plain/prog
```

Terminal 2:

```
gdb -q build/plain/prog
(gdb) target remote | vgdb
(gdb) continue
```

When Memcheck stops on the error, inspect the item it was looking at: `print i`,
`print it[i]`, and its validity bits:
`eval "monitor get_vbits %p %d", &it[i], (int)sizeof it[i]`.

Fix the bug. Write `ANSWER.md`: what `print` said, what `get_vbits` said, and why they
disagree; and why the ASan build passed.

## Constraints

- Exactly one defect, in `items_new`. Keep `malloc` (do not switch to `calloc`): fix the
  logic so every field is assigned.

## Verify

`make check` passes. The `valgrind` stage is the one that matters.
