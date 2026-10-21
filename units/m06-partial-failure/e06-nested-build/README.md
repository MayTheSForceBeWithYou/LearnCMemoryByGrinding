# M06 e06: All or nothing, in a loop `[WRITE]` `[FAULT]`

## Goal

Implement

```c
struct deck { char *title; char **cards; size_t n; };

struct deck *deck_create(const char *title, const char *const *cards, size_t n);
void         deck_destroy(struct deck *d);
```

`deck_create` copies the title and every card string. The caller's strings are borrowed. The result
owns `n + 3` allocations: the struct, the title, the pointer array, and `n` strings. If any of
them fails, nothing survives.

**The starter will not build until you write both definitions.** The link error is intentional.

## Constraints

- Allocation failure while copying card `i` must free exactly the cards copied so far, plus the
  array, the title, and the struct.
- `n == 0` is valid: an empty deck whose `cards` may be NULL. Decide, and document it in the header.
- `deck_destroy(NULL)` is a no-op.
- Clean on every `FAIL_AT`. The harness builds a 10-card deck, so the sweep has more than a dozen failure points.

## How to verify

- `make fault` and `make check`.

<details><summary>Hint</summary>

If the pointer array starts out zeroed, freeing "the cards copied so far" and freeing "all `n`
slots" become the same loop.

</details>
