# M05 e03: All-or-nothing out-parameter `[WRITE]`

## Goal

Implement

```c
struct pair { char *key; char *value; };

int  parse_pair(const char *line, struct pair **out);   /* "key=value" */
void pair_destroy(struct pair *p);
```

`parse_pair` returns 0 and stores a new `struct pair` (owning copies of both strings) in
`*out`, or returns -1 on malformed input (no `=`, empty key) or allocation failure.
The harness initialises `*out` to a sentinel pointer before each call and checks that
failure leaves it unchanged.

**The starter will not build until you write both definitions.** The link error is intentional.

## Constraints

- On any failure: `*out` untouched, and nothing leaked. There are three allocations in the success
  path, so there are three places to fail.
- `pair_destroy(NULL)` is a no-op.
- `line` is borrowed for the duration of the call. Don't keep pointers into it.
- Write the contract comment for both functions in `pair.h`.

## How to verify

- `make check`: checks, ASan, and Valgrind clean.
- Optional preview of M06: `make fault` should already pass if your failure paths are right.
