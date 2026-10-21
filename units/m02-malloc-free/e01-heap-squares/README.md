# M02 e01: Heap squares `[WRITE]`

## Goal

Implement

```c
long *squares(size_t n);            /* returns {0, 1, 4, ..., (n-1)^2} */
long  sum_and_free(long *v, size_t n);
```

`squares` returns a new heap array of `n` elements where element `i` is `i * i`.
`sum_and_free` adds up the elements and then frees the array. The harness calls them for
several `n`, including `n == 0` and a large `n`.

**The starter will not build until you write both definitions** in `main.c` below the
marked line. The link fails with `undefined reference`, and that's intentional.

## Constraints

- Size the allocation with `sizeof *p`. No type name inside `malloc(...)`.
- On allocation failure, `squares` returns NULL without printing. `main` already turns that
  into a one-line stderr message and `exit(EXIT_OOM)`.
- `n == 0`: you decide whether `squares` returns NULL or a unique pointer, but
  `sum_and_free(squares(0), 0)` must return 0 without leaking and without undefined
  behaviour either way. Write your choice in a comment above `squares`.
- `sum_and_free` must accept NULL with `n == 0`.
- Assume `n * n` fits in a `long` for every `n` the harness uses. Overflow in the
  *size* computation is M03's job.

## How to verify

- `make check`: checks, ASan, LeakSanitizer, and Valgrind all clean.

<details><summary>Hint</summary>

`man 3 malloc` says what `malloc(0)` may return. Both answers it allows have to work with
`free`.

</details>
