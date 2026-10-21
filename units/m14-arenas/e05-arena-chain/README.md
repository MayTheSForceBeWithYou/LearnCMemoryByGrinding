# m14-e05-arena-chain `[FAULT]`

## Goal

Implement the growable arena declared in `chain.h`: a chain of blocks, newest first. When
the newest block cannot fit a request, a new block is allocated and linked in front. An
earlier allocation never moves.

## Constraints

- Every returned pointer is `max_align_t`-aligned. Get that from the `max_align_t data[]`
  flexible member, not from arithmetic on the block's address.
- A request larger than `block_size` gets a block of exactly its rounded size.
- All size arithmetic is overflow-checked: rounding, and `sizeof(Block) + cap`.
- On allocation failure, return NULL and leave the chain valid, with every earlier
  allocation intact.
- `chain_init` allocates nothing.

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.

## Verify

`make check` passes, including the fault sweep.
