# M07 e01: Your own strdup `[WRITE]`

## Goal

Implement, without calling `strdup`, `strndup`, `strcpy`, or `strcat`:

```c
char *my_strdup(const char *s);
char *my_strndup(const char *s, size_t n);
```

Same contracts as the POSIX functions: a new heap copy the caller frees, NULL on allocation
failure, and `my_strndup` copies at most `n` characters and always NUL-terminates. The harness
compares against the real functions for a set of inputs, including `""`, `n == 0`, `n` larger
than the string, and a `char` array that is **not** NUL-terminated within `n` bytes.

**The starter will not build until you write both definitions.** The link error is intentional.

## Constraints

- Allocate exactly length + 1 bytes, where length is the length of the *result*.
- `my_strndup` must not read past `s[n - 1]` when no NUL appears in the first `n` bytes. ASan
  checks this: the harness passes an exact-size heap array.
- `main.c` defines `_POSIX_C_SOURCE` so it can call the real functions for comparison. Your file
  needs it only if you call POSIX functions yourself.

## How to verify

- `make check`.
