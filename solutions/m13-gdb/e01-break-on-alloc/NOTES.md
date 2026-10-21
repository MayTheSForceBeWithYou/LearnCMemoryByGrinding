# m13-e01-break-on-alloc: notes

The reference script is `trace.gdb`. `make gdb-trace` runs it, and `capture.txt` is its real output.

| call | old pointer | size | pushing `x` | moved? |
|---|---|---|---|---|
| 1 | `(nil)` | 4 | 0 | (first allocation) |
| 2 | `0x555555559010` | 8 | 1 | no |
| 3 | `0x555555559010` | 16 | 4 | no |
| 4 | `0x555555559010` | 32 | 16 | no |
| 5 | `0x555555559010` | 64 | 64 | no |

The old pointer is the same in every call, and `v.data` is still `0x555555559010` at
POINT B, so glibc grew the block **in place** each time. The block is the last chunk before
top, so it can simply extend into top. Do not rely on that: any allocation in between
would force a move, and then every pointer saved from before is dangling (M04).

There are no `free` calls before POINT B: a successful `realloc` that grows in place frees
nothing, and one that moves frees the old block *inside* glibc. A breakpoint on `free`
would not see that either, because libc calls its internal implementation directly.

At POINT B: `{data = 0x555555559010, len = 10, cap = 16}`, elements
`{0, 1, 4, 9, 16, 25, 36, 49, 64, 81}`.

`v.data[9]@2` prints `{81, 135121}`. The second element is `v.data[10]`, inside the block
(capacity 16) but never written. gdb shows whatever bits are there. Here 135121 is
`0x20fd1`, the low half of a heap size field. While the block had capacity 8 (a 48-byte chunk),
the top chunk's header began right after it, and offset 40 of the block was top's size:
0x21000 - 0x30 = 0x20fd0, plus the "previous in use" bit. It is stale allocator metadata, not program data.
