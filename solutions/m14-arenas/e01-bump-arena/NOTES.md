# m14-e01-bump-arena: notes

`arena_alloc` rejects `size == 0` and any `size > SIZE_MAX - (ALIGN - 1)` *before* rounding,
so `(size + ALIGN - 1) & ~(ALIGN - 1)` cannot wrap. `SIZE_MAX - 8` would otherwise round to
0 and "fit". The fit test is `rounded > cap - used`, which cannot wrap because
`used <= cap` is an invariant.

The returned pointers are aligned because `base` comes from `malloc` (16-aligned) and
`used` is always a multiple of 16. Both conditions are needed. An arena carved from a
`char buf[256]` on the stack would need `alignas(max_align_t)` on the buffer.

The 5-byte request costs 16 bytes of arena. That is the price of handing out
any-type-aligned memory without knowing the type. A typed API (`arena_alloc(a, size, align)`)
can pack tighter, and production arenas usually take the alignment as a parameter.
