# M04 e06: A buffer that survives every failure `[FAULT]`

## Goal

Implement a byte buffer:

```c
struct bbuf { unsigned char *data; size_t len, cap; };

int  bbuf_reserve(struct bbuf *b, size_t extra);          /* room for len + extra */
int  bbuf_append(struct bbuf *b, const void *src, size_t n);
int  bbuf_shrink_to_fit(struct bbuf *b);
void bbuf_free(struct bbuf *b);
```

All `int` functions return 0 on success and -1 on failure. The harness appends data of
many sizes (including 0 bytes and appends larger than the current capacity), shrinks,
appends again, and checks contents. Under `make fault`, every allocation call is failed in turn.

**The starter will not build until you write the four definitions.** The link error is intentional.

## Constraints

- Any failure leaves `*b` exactly as it was before the call, with contents intact.
- `main` handles a -1 by calling `bbuf_free` and exiting `EXIT_OOM`. Every `FAIL_AT` run must
  exit 0 or 3, with no leak and no sanitizer report.
- `len + extra` must be checked for overflow, as must the growth computation.
- `bbuf_shrink_to_fit` on an empty buffer must free the block and leave `data == NULL`
  rather than calling `realloc` with 0. A failed shrink is not an error the caller sees.
  Decide what it returns, and document that above the function.
- `bbuf_append(b, NULL, 0)` is valid and does nothing.

## How to verify

- `make fault`: the full `FAIL_AT=1..N` sweep.
- `make check`: checks, ASan, Valgrind, and the sweep.

<details><summary>Hint</summary>

If `append` does its own `realloc` instead of going through `reserve`, you have two
failure paths to get right instead of one.

</details>
