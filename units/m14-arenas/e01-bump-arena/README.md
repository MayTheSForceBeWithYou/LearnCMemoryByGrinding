# m14-e01-bump-arena `[WRITE]`

## Goal

Implement the bump arena declared in `arena.h`: `arena_init`, `arena_alloc`,
`arena_reset` and `arena_free`. Every pointer `arena_alloc` returns must be aligned to
`_Alignof(max_align_t)`, so that it is valid for any object type, like `malloc`'s.

## Constraints

- `arena_init` makes the only `malloc` call. `arena_alloc` never calls the allocator.
- Round each request up to the alignment. The rounding and the fit test must not wrap for
  any `size`: the harness asks for `SIZE_MAX` and `SIZE_MAX - 8`.
- `used` counts padding too, so `cap - used` is always the exact free space.
- `<stdalign.h>` gives you `alignof`. `max_align_t` is in `<stddef.h>`, and `SIZE_MAX`
  in `<stdint.h>`.

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.

## Verify

`make check` passes.
