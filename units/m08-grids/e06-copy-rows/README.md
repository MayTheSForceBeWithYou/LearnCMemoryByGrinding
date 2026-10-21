# m08-e06-copy-rows `[FAULT]`

## Goal

Deep-copy a table of strings: a new NULL-terminated table and a new copy of every string.
Do it all-or-nothing. Either the caller gets a complete copy, or gets NULL and nothing leaked.

```c
char **copy_rows(const char *const *src, size_t n);   /* src has n strings */
void   free_rows(char **rows);                         /* accepts NULL */
```

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- One table allocation plus one per string. `strdup` is allowed (the fault wrapper sees it).
- If any allocation fails, `copy_rows` frees everything it allocated so far and returns NULL.
- `n == 0` returns a table containing only the terminating NULL.
- The harness exits with `EXIT_OOM` (3) and a one-line stderr message when `copy_rows` returns
  NULL, after freeing anything *it* owns. That part of `main.c` is written. Your job is the library side.
- Under strict `-std=c17`, `strdup`/`strndup` are declared only because `main.c` starts with
  `#define _POSIX_C_SOURCE 200809L`, ahead of every `#include`. Keep it there.

## Verify

```bash
make fault      # FA_COUNT run, then FAIL_AT=1..N under ASan
make check      # plain + ASan + Valgrind + the sweep
```

Every `FAIL_AT` run must either print `m08-e06-copy-rows: all checks passed` or exit 3,
with no sanitizer report and no leak.

<details><summary>Hint</summary>

Sweep by hand once before trusting `make fault`. The harness allocates the `n == 0` table first
(`FAIL_AT=1`), then the `n == 5` table (`FAIL_AT=2`), so `FAIL_AT=3` is the first string. Failing a
table and failing a string exercise different cleanup paths. The last allocation inside `copy_rows` is where an
off-by-one in the cleanup loop shows up.

</details>
