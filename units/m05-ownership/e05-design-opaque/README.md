# M05 e05: An opaque string stack `[DESIGN]`

## Goal

Design and implement an opaque stack of strings:

```c
typedef struct sstack sstack;
```

The harness in `main.c` calls these functions, and their names and signatures are fixed:

```c
sstack     *sstack_create(void);
void        sstack_destroy(sstack *s);
int         sstack_push(sstack *s, const char *str);   /* copies str */
char       *sstack_pop(sstack *s);                     /* transfers ownership */
const char *sstack_peek(const sstack *s);              /* borrows */
size_t      sstack_size(const sstack *s);
```

`sstack.h` ships with the typedef and these prototypes, and nothing else. You write a full
contract comment above each one. You also write `sstack.c`: the struct definition and the
implementation. `main.c` must not be able to see the struct's fields, so the definition
must not go in the header.

**The starter will not build until you write the definitions in `sstack.c`.** The link
fails with `undefined reference`, and that's intentional.

## Constraints

- `sstack_pop` and `sstack_peek` on an empty stack return NULL. Your contract states it.
- `sstack_peek`'s result: state exactly which operations invalidate it.
- `sstack_destroy` frees every string still on the stack and accepts NULL.
- A failed push leaves the stack unchanged and the caller still owns `str` (it was
  borrowed anyway).
- The stack grows without limit (M04's growth rules apply).

## How to verify

- `make check`: checks, ASan, and Valgrind clean.
- Opacity test: temporarily add `(void)s->len;` (or any field name) to `main.c`. The build
  must fail with an "incomplete type" error. Remove the line afterwards.
