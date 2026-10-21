# m08-e01-split-words `[WRITE]` (S)

## Goal

Split a line into words and return them as an owned, NULL-terminated table of heap strings,
plus the matching destroy function. A word is a maximal run of non-whitespace characters
(as `isspace` defines it).

```c
char **split_words(const char *line, size_t *count_out);
void   free_words(char **words);
```

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- The caller owns the result: one table plus one block per word. `free_words` releases all of it
  and accepts NULL.
- The table is NULL-terminated, and `*count_out` (if `count_out` is not NULL) receives the word count.
- A line with no words returns a valid table holding only the terminating NULL, not NULL.
  NULL means allocation failure.
- On allocation failure, return NULL and leak nothing.
- `line` is not modified. Do not use `strtok`.
- Size the table exactly (count first) or grow it. Either is fine, but no fixed maximum word count.
- Under strict `-std=c17`, `strdup`/`strndup` are declared only because `main.c` starts with
  `#define _POSIX_C_SOURCE 200809L`, ahead of every `#include`. Keep it there.

## Verify

```bash
make run        # checks pass
make asan       # silent
make valgrind   # exit 0, no definite or indirect leaks
make check      # all of the above
```

The harness prints `m08-e01-split-words: all checks passed`.

<details><summary>Hint</summary>

Pass `(unsigned char)c` to `isspace`. A plain `char` holding a byte above 127 is negative on
x86, and passing a negative value other than `EOF` to `isspace` is undefined.

</details>
