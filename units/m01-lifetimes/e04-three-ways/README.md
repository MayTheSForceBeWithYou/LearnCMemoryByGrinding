# M01 e04: Three ways to return a string `[WRITE]`

## Goal

Implement the same operation three times with three different lifetime strategies:
format a position as `"(x, y)"`, for example `"(3, -12)"`.

```c
const char *pos_static(int x, int y);                       /* static buffer */
char       *pos_into(char *dst, size_t cap, int x, int y);   /* caller's buffer */
char       *pos_heap(int x, int y);                         /* new heap block */
```

The prototypes are in `pos.h`, with a comment block above each one that you must fill
in. The harness in `main.c` tests all three, including a test that keeps two results
alive at once.

**The starter will not build until you write the definitions in `pos.c`.** The link
fails with `undefined reference to 'pos_static'` and the others. That is intentional.

## Constraints

- `pos_static`: one static buffer. The comment must say what a second call does to the
  first result.
- `pos_into`: never writes more than `cap` bytes, including the NUL. If the text does not
  fit, return NULL and leave `dst` holding a valid (possibly truncated) string. If
  `cap == 0`, don't write at all.
- `pos_heap`: allocates exactly enough bytes for the result and its NUL, not a fixed
  guess. Returns NULL if allocation fails. The comment must say who frees the result.
- All three must handle `INT_MIN` correctly.
- In `ANSWER.md`, write two or three sentences on which version you'd ship in a library,
  and why.

## How to verify

- `make check`: checks, ASan, and Valgrind all clean, with no leaks from the heap version.

<details><summary>Hint</summary>

`snprintf` returns the number of characters it *would* have written, whether or not they
fit. One call with a NULL buffer and size 0 tells you the exact length. M07 builds on this
idea.

</details>
