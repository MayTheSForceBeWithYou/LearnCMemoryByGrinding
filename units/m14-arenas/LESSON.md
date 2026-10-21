# M14: Arenas and pools

## Organizer

`malloc` is a general-purpose allocator. Any size, any order of frees, and each block
carries its own header. Two simpler allocators beat it when the program's allocation
pattern is known in advance. A **bump arena** hands out pieces of one big block and frees
them all at once. A **pool** hands out fixed-size slots and takes them back one at a time
in O(1). This unit builds both, and teaches the one thing every custom allocator must get
right: **alignment**.

You need M02-M04 (allocation and size arithmetic) and M11 (tools). The demo numbers below
come from `lesson-demo/capture-run.txt`.

## Foundations

### Alignment

Every complete object type has an *alignment requirement*: a power of two, and its address
must be a multiple of it (C17 6.2.8). On this machine (x86-64 Linux, GCC 16):

```
alignof(char)        = 1
alignof(int)         = 4
alignof(double)      = 8
alignof(long double) = 16
alignof(max_align_t) = 16
sizeof(max_align_t)  = 32
malloc(1) % 16       = 0, 0
distance between them = 32 bytes
```

`max_align_t` (`<stddef.h>`) is a type whose alignment is the largest *fundamental*
alignment, the strictest any ordinary type needs. `malloc` must return memory suitably
aligned for any type with fundamental alignment (C17 7.22.3), so glibc's results are
multiples of 16. Two `malloc(1)` results are 32 bytes apart: the minimum chunk (M12).
`sizeof(max_align_t)` is 32 because it is a struct holding a `long long` and a `long double`.
Its size is not the alignment. Use `alignof`.

Accessing an object through a misaligned pointer is undefined behaviour: the conversion
itself is undefined if the result is misaligned (C17 6.3.2.3p7). x86-64 hardware tolerates
most misaligned loads, which is why this bug survives testing. But the compiler may assume
alignment and emit instructions that require it (SSE `movaps` for 16-byte types, for
example), and other architectures fault outright. `-fsanitize=alignment`, part of the
course's `-fsanitize=undefined`, checks every access through a pointer and reports
`member access within misaligned address ...` or `store to misaligned address ...`.
ASan and Memcheck do not check alignment.

### Rounding up

For a power-of-two alignment `a`, the smallest multiple of `a` that is `>= n` is

```c
(n + a - 1) & ~(a - 1)
```

Adding `a - 1` crosses the next boundary unless `n` is already on one. The mask clears
the low bits. Two traps:

- `n + a - 1` can wrap. Check `n > SIZE_MAX - (a - 1)` first, or a huge request rounds
  to a tiny one and "fits".
- `a` must be a power of two, or the mask is meaningless. `alignof` values always are.

An arena that rounds every request to `alignof(max_align_t)` returns pointers that are
valid for any type, exactly like `malloc`. That is the contract callers assume when they
write `Item *it = arena_alloc(&a, sizeof *it);`.

### Bump arena

```
base                         base+used                     base+cap
|[obj 1][pad][obj 2 ][obj 3  ]|.............free...........|
```

`arena_alloc(size)`: round `size`, check it fits in `cap - used`, return `base + used`, and
add the rounded size to `used`. There is no per-object header and no `free` per object.
`arena_reset` sets `used = 0`, and `arena_free` releases the single block. Cost per
allocation: an add, a compare, a mask.

Write the fit test as `rounded > cap - used`, not `used + rounded > cap`. The second form
wraps for a huge `rounded`, and `used <= cap` always holds, so the subtraction cannot.

A **chain** of arenas (M14 e05) removes the fixed capacity: when the current block is full,
allocate a new one and link it in front. Allocations never move, so earlier pointers stay
valid (unlike `realloc`ing one big block, M04). A flexible array member of type
`max_align_t` makes the block's data area aligned without hand arithmetic, because the
compiler aligns the member and pads the header.

### Pool with a free list

A pool serves objects of one size. `pool_init` allocates `count` slots in one block. Each
*free* slot stores a pointer to the next free slot **inside itself**, so the free list costs
no memory:

```
free_list -> [slot 2 | next=slot 0] -> [slot 0 | next=NULL]      slot 1: in use
```

`pool_get` pops the head and `pool_put` pushes the slot back: O(1), LIFO. A slot must be
large enough to hold a `void *` and must keep every slot aligned, so the slot size is
`max(objsize, sizeof(void *))` rounded to `alignof(max_align_t)`. `count * slot` must be
checked for overflow before the `malloc` (M03).

Reading the `next` pointer out of a slot is only valid while it is free. A use-after-`put`
overwrites the free list. That is the pool version of corrupted tcache metadata, with no
glibc check to catch it.

### What the tools can see

To ASan and Memcheck an arena is **one** `malloc` block. An overflow from one arena object
into the next, or a use of a pool slot after `pool_put`, is an access to memory the tool
considers valid, so neither reports it. Leaks also become coarse: forgetting `arena_free`
leaks one block, but forgetting an object inside the arena is not a leak to anyone. Both
tools have client APIs to annotate custom allocators (ASan's `ASAN_POISON_MEMORY_REGION` in
`<sanitizer/asan_interface.h>`, Valgrind's `VALGRIND_MEMPOOL_ALLOC` family in
`<valgrind/valgrind.h>`). Production arenas use them, and this unit does not.

### When each wins

| Pattern | Best fit | Why |
|---|---|---|
| many objects, one shared lifetime (parse tree, one request, one frame) | arena | O(1) bump, free all at once, no per-object header, contiguous |
| many same-size objects, independent lifetimes, bounded count | pool | O(1) get/put, no fragmentation, no header |
| mixed sizes and independent lifetimes | `malloc` | that is the general case it is built for, and tools see every object |
| one long-lived growing array | `malloc` + `realloc` | an arena cannot grow an object in place |

glibc's tcache makes small `malloc`/`free` pairs fast already. An arena's real wins are the
single `free` (no traversal at teardown, no forgotten nodes) and the missing headers. A
16-byte node costs a 32-byte chunk from `malloc`, and 16 bytes in an arena. Measure before
you claim a speedup. M14 e04 has you predict what Valgrind's heap summary says about two
strategies, and why the "bytes allocated" figure does not show the header cost.

## Check yourself

1. An arena returns `base + used` with no rounding. A caller allocates 5 bytes, then a
   `double`. Which address does the `double` get, relative to `base`, and is that valid?
2. Why is `if (used + rounded > cap)` wrong, when `if (rounded > cap - used)` is right?
3. A pool slot size is `objsize` rounded to 16, and `objsize` is 1. What else must the slot
   size guarantee?
4. Why does ASan not report a write past the end of an arena object?

<details><summary>Answers</summary>

1. `base + 5`. `malloc`'s `base` is 16-aligned, so the `double` is at an address 5 more
   than a multiple of 16. That is misaligned for `alignof(double) == 8`: undefined
   behaviour, which UBSan reports at the first access.
2. `used + rounded` can wrap for a huge `rounded`, and pass. `cap - used` cannot wrap,
   because `used <= cap` is an invariant.
3. That it can hold the free-list pointer: at least `sizeof(void *)`. 16 already is, but
   say so in code (`max(objsize, sizeof(void *))`) rather than relying on it.
4. The whole arena is one valid allocation as far as ASan knows. Only the arena's end is
   followed by a redzone. Manual poisoning is needed to see more.
</details>

## Exercises

| Exercise | Tag | Size | Topic |
|---|---|---|---|
| e01-bump-arena | `[WRITE]` | S | bump arena, rounding to `alignof(max_align_t)`, overflow-safe fit test |
| e02-misaligned | `[BUG]` | S | an arena without rounding; UBSan's alignment check |
| e03-pool-freelist | `[WRITE]` | M | fixed-size pool, intrusive free list |
| e04-heap-usage | `[PREDICT]` | S | malloc-per-node vs one block, as Valgrind counts them |
| e05-arena-chain | `[FAULT]` | M | growable chain of arenas, flexible `max_align_t` member |
