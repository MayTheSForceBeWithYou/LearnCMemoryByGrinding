# m14-e03-pool-freelist `[WRITE]`

## Goal

Implement the fixed-size pool declared in `pool.h`: one block of slots, with free slots
threaded into a singly linked list stored *inside the free slots*. `pool_get` and
`pool_put` are O(1).

## Constraints

- Exactly one `malloc`, in `pool_init`. No other allocation, and no separate array for the
  free list.
- Slot size: `objsize`, at least `sizeof(void *)`, rounded up to `alignof(max_align_t)`.
  Check for overflow in the rounding and in `count * slot` before allocating.
- On failure, `pool_init` leaves `*p` safe to pass to `pool_destroy`.
- The free list must be LIFO (the harness checks it). The order in which a fresh pool hands
  out its slots is up to you: choose one and say why in a comment.
- Store and load the `next` pointer through `void **` or `memcpy`. A struct overlay is
  fine too. Say which and why.

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.

## Verify

`make check` passes.

<details><summary>Hint</summary>

Building the initial list from the last slot to the first makes `pool_get` hand out slots
in address order.
</details>
