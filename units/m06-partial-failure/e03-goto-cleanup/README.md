# M06 e03: One way out `[FAULT]`

## Goal

Implement a loader for a tiny score file (`scores.txt` ships in the folder):

```
title line
<int>
<int>
...
```

```c
struct scores { char *title; int *vals; size_t n; };

int  scores_load(const char *path, struct scores **out);
void scores_destroy(struct scores *s);
```

`scores_load` opens the file, reads lines into a heap line buffer (`fgets` into a buffer you
allocate, not `getline`), copies the title, collects the numbers in a growing array, builds the
result, closes the file, and stores the result in `*out`. That's five resources: the `FILE *`, the
line buffer, the title copy, the number array, and the result struct.

**The starter will not build until you write both definitions.** The link error is intentional.

## Constraints

- **One exit:** every path, success included, leaves through a single `cleanup:` label.
- On failure (unopenable file, malformed number, allocation failure) return -1, leave `*out`
  unchanged, and leak nothing, including the `FILE *`.
- On success, the line buffer and the `FILE *` are released, and the title and number array belong to
  the result.
- `getline` is off-limits here. Its internal allocation is invisible to the fault injector (see
  the lesson), so the sweep couldn't test it.
- Lines are at most 127 characters. Longer lines are malformed.

## How to verify

- `make fault`: every `FAIL_AT` exits 0 or 3, clean.
- `make check`: checks (which include a missing file and a malformed file), ASan, Valgrind, and the sweep.

<details><summary>Hint</summary>

Initialise every resource variable to NULL at the top. When ownership moves into the result,
NULL the local. Then `cleanup:` can release everything unconditionally.

</details>
