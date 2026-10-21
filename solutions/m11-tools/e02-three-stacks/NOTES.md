# m11-e02-three-stacks: notes

- **Accessed at** `main.c:41`: `first[0]` reads 1 byte of a freed block.
- **Freed by** `line_set` line 16 called from `main.c:38`: the second `line_set`.
- **Allocated by** `line_set` line 16 called from `main.c:35`: the first `line_set`, where
  `realloc(NULL, 6)` behaves as `malloc(6)` (the report says "6-byte region").

`realloc` (line 16) may allocate a new block, copy, and free the old one, so a single
call site can appear as both allocator and freer. The second call grows to 39 bytes and
ASan's `realloc` always moves. `first` was a borrowed pointer into the old block, and any
call that may reallocate ends that borrow.

**Fix.** Copy what you need before the call: `char first = l.buf[0];` and compare
`first == 's'`. Keeping a pointer is wrong however the check is phrased.

**Why the plain build passed.** glibc can extend a chunk in place when it sits next to the
top of the heap, so `realloc` returned the same address and `first` still pointed into live
memory. Correctness must not depend on that.
