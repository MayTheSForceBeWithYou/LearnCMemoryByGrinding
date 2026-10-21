# m14-e02-misaligned: notes

**Bug.** `arena_alloc` hands out `base + used` with no rounding. After the 5-byte `code`,
`item` is at `base + 5`. `Item` contains a `double`, so `alignof(Item) == 8`, and `base + 5`
is misaligned (`base` is 16-aligned from `malloc`).

**Mechanism.** Forming the misaligned `Item *` is already undefined (C17 6.3.2.3p7).
UBSan's `-fsanitize=alignment` check fires at the first access through it,
`item->price = 9.75` (real output):

```
main.c:55:17: runtime error: member access within misaligned address 0x793b042e0105 for type 'struct Item', which requires 8 byte alignment
```

The address ends in `5`: `base` ends in `0` and the offset is 5. With `-fno-sanitize-recover=all` the run stops there. x86-64 performs
an unaligned 8-byte `movsd` without complaint, so the plain build works, and Memcheck checks
addressability and definedness only. Every byte of `base + 5` to `base + 20` is inside the
4096-byte block, which is why ASan is silent too.

**Fix.** Round each request to `alignof(max_align_t)` with the overflow check first:

```c
const size_t align = alignof(max_align_t);
if (size == 0 || size > SIZE_MAX - (align - 1))
    return NULL;
size_t rounded = (size + align - 1) & ~(align - 1);
if (rounded > a->cap - a->used)
    return NULL;
```

and add `rounded` to `used`. This needs `<stdalign.h>`, `<stddef.h>` and `<stdint.h>`.
