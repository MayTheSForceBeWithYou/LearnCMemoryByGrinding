# m10-e06-list-from-words `[FAULT]`

## Goal

Build a list from an array of words in their original order, all-or-nothing.

```c
Node *list_from_words(const char *const *words, size_t n);  /* NULL on failure; also NULL when n == 0 */
void  list_destroy(Node **head);                            /* provided, already correct */
```

`Node` is the same as e01's: an owned `name` and a `next`.

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- On any allocation failure, everything allocated by this call is freed and NULL is returned.
- The result preserves order. O(n) total: no walking to the tail on every append.
- The harness exits `EXIT_OOM` (3) with a one-line stderr message when it gets NULL for a
  non-empty input. That's written.
- **n == 0 also returns NULL.** That's ambiguous by design. Write in `NOTES.md` how the harness tells
  the two cases apart, and how you would change the API if you could.
- Under strict `-std=c17`, `strdup` is declared only because `main.c` starts with
  `#define _POSIX_C_SOURCE 200809L`, ahead of every `#include`. Keep it there.

## Verify

```bash
make fault
make check
```

Every `FAIL_AT` run prints `m10-e06-list-from-words: all checks passed` or exits 3, with no leak
and no sanitizer report.

<details><summary>Hint</summary>

Two allocations per word means two failure points per word. A failure on the `name` copy happens
after the node exists. Make sure that node is either already in the list you clean up, or freed on its own.

</details>
