# M02 e05: Copy what you borrowed `[WRITE]`

## Goal

Implement

```c
int *dup_ints(const int *src, size_t n);
```

`src` is borrowed: the caller keeps ownership and may free or change it right after the
call. The result is a new heap block with the same `n` values, owned by the caller.

The harness checks the copy, then frees or overwrites `src` and checks that the copy is
unaffected. It also checks `n == 0` and the NULL-on-failure contract.

**The starter will not build until you write the definition.** The `undefined reference`
link error is intentional.

## Constraints

- One allocation per call, sized with `sizeof *p`.
- Return NULL on allocation failure, without printing.
- If `n == 0`, `src` may be NULL. Your function must not dereference it then.
- Write a two-line ownership comment above the definition: what is borrowed and what is
  returned.

## How to verify

- `make check`: checks, ASan, and Valgrind all clean.
