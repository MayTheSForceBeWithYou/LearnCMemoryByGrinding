# M04 e01 notes

- The first push calls `realloc(NULL, 8 * sizeof(int))`, which is `malloc`. There's no special case, because `{0}` already *is* the empty state.
- The overflow guard covers the byte count, `newcap * sizeof(int)`, not just the element count. `SIZE_MAX / 2 / sizeof` is the largest
  capacity that can still be doubled and multiplied.
- Commit order: `tmp` is checked, then `data`, then `cap`. If `cap` were updated first and `realloc` failed, the next push would write
  past the real block. That's e02's second bug.
- `realloc` is never called with 0: `newcap` starts at 8 and only grows.
- Common mistake: `v->data = realloc(v->data, ...)`. It leaks on failure, and you'll see it in e02 under `make fault`.
