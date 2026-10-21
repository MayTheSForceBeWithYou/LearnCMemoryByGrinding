# m13-e02-far-corruption `[BUG]`

## Goal

`main.c` makes a `Bag` of three items, gives it a label, and sums the items. Both checks
fail, and ASan reports a heap overflow in `bag_sum`, a function that is correct.

Find the bug **with a watchpoint**: find out *which instruction* changes `b.count`, and who
called it. Then fix it. Write `ANSWER.md`: the watchpoint session (commands and the stop),
the mechanism (bytes and offsets), and why ASan reported `bag_sum` instead of the real bug.

```
make build
gdb -q build/plain/prog
(gdb) break bag_label
(gdb) run
(gdb) watch -l b->count
(gdb) continue
(gdb) bt
```

## Constraints

- Exactly one defect. The struct layout stays as it is (`name` is 8 bytes).
- Labels longer than 7 characters are truncated to fit `name`. That is the intended
  behaviour.

## Verify

`make check` passes.

<details><summary>Hint</summary>

The watchpoint stops in a libc frame with no name. Frame `#1` is the one to read.
</details>
