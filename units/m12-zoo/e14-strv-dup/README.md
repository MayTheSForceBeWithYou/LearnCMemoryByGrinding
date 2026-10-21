# m12-e14-strv-dup `[FAULT]`

## Goal

Implement `strv_dup()` and `strv_free()` as documented in `main.c`: a deep copy of a
NULL-terminated vector of strings, which must survive an allocation failure at every
allocation point without leaking.

## Constraints

- Exactly one allocation for the array and one per string.
- On failure, free everything this call allocated, in reverse order, and return NULL.
- `strdup` needs `#define _POSIX_C_SOURCE 200809L` before the includes.

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.

## Verify

`make check` passes, including the fault sweep: every `FAIL_AT` exits 0 or 3, with no
LeakSanitizer report.
