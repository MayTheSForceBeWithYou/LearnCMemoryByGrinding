# m14-e05-arena-chain: notes

**Alignment for free.** `Block` is `{next, cap, used}` (24 bytes) followed by
`max_align_t data[]`. The compiler pads the header so `data` sits at offset 32
(`sizeof(Block) == 32`), a multiple of 16. `malloc` returns 16-aligned blocks, so `data` is
16-aligned, and every rounded offset into it stays aligned.

**The failure path is trivial by design.** The new block is fully initialised and linked
only after `malloc` succeeds. A NULL leaves `c->head` untouched, so the chain stays valid
and nothing leaks. The harness makes 6 block allocations, so the sweep runs `FAIL_AT=1..6`,
and each run exits 3 after `chain_destroy` frees what was built.

**Counting blocks.** 20 rounds to 32, two per 64-byte block: 10 requests give 5 blocks.
The 8000-byte request exceeds `block_size` and gets an 8000-byte block of its own. That
block becomes the head and is full at once, so the *next* small request would open yet
another block, even though nothing older has space either. A smarter chain links
oversized blocks *behind* the head, so the head keeps serving small requests. That is a
reasonable extension and not required here.
