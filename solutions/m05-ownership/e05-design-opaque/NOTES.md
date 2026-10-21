# M05 e05 notes

- `typedef struct sstack sstack;` in the header, `struct sstack { ... }` in the `.c`. `main.c` can hold an `sstack *` but can't
  evaluate `sizeof(sstack)` or `s->len`. Adding `(void)s->len;` to `main.c` gives "invalid use of incomplete typedef".
- **peek's validity:** a push may `realloc` the `items` array, but the *strings* are separate blocks that never move. So the borrowed
  `const char *` survives pushes and dies on pop (the popped string now belongs to the caller, who may free it) or destroy. Say exactly that.
  A vaguer "until the next modification" is safe but less useful.
- **pop is a transfer:** after `--s->len`, the stack no longer references the string, so `destroy` won't free it. The caller must.
- **push order:** growing first and copying second means a failed copy leaves a larger but otherwise unchanged stack. That's an acceptable
  "unchanged" state, because capacity isn't observable through the API. That's another benefit of opacity.
