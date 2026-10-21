# m09-e05-strvec `[DESIGN]` `[FAULT]` (M)

## Goal

A vector of strings that **owns** them. `strvec.h` ships with the prototypes and no comments:

```c
typedef struct { char **data; size_t len; size_t cap; } StrVec;

bool  strvec_push_copy(StrVec *v, const char *s);
bool  strvec_push_take(StrVec *v, char *s);
char *strvec_pop(StrVec *v);
const char *strvec_get(const StrVec *v, size_t i);
void  strvec_destroy(StrVec *v);
```

First write the ownership contract as header comments, one per function: who owns what
before and after the call, on success **and on failure**, and how long a returned pointer stays valid.
Then implement it so that the harness passes.

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- The contract is part of the deliverable. The harness tests behaviour, and the reviewer reads the header.
- `strvec_push_take` must answer "growth failed: who frees `s`?" The harness assumes the answer
  is **the caller still owns `s` after a failed take**, so it frees `s` itself on that path. The harness grades
  this: it forces a take to fail on a vector whose growth overflows, then reads and frees `s`. Your contract must
  say so, and in `NOTES.md` give one reason an API might choose the opposite.
- Every allocation failure must be survivable: `make check` runs the fault sweep (`FAIL_AT=1..N`), and every
  run must exit 0 or 3 with no sanitizer or leak report.
- `strvec_get` returns a borrowed pointer. State in the contract which calls invalidate it.
- `strvec_destroy` frees every string, then the buffer, and accepts a zeroed `StrVec`.
- `StrVec v = {0};` is valid and empty.
- Under strict `-std=c17`, `strdup`/`strndup` are declared only because `main.c` starts with
  `#define _POSIX_C_SOURCE 200809L`, ahead of every `#include`. Keep it there.

## Verify

```bash
make check
```

The harness prints `m09-e05-strvec: all checks passed`; ASan and Valgrind are silent, and every
`FAIL_AT` run of the sweep passes or exits 3.

<details><summary>Hint</summary>

Write the failure half of each contract before the success half. It's the half you'll be tempted
to leave vague. To exercise it before the full sweep, run `make build-fault`, then
`FAIL_AT=k ./build/fault/prog` for small `k`.

</details>
