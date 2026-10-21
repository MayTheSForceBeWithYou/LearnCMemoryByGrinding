# m11-e07-split-fields `[FAULT]`

## Goal

Implement `split_fields()` and `free_fields()` as specified in `harness.h`: split a line at
a separator into an array of heap strings, NULL-terminated, with exact empty-field semantics.
The functions must survive an allocation failure at **every** allocation point.

The harness also calls `mtrace()`. Use it once your checks pass, to see glibc's own
record of what your success path leaves behind:

```
make
MALLOC_TRACE=build/mtrace.log LD_PRELOAD=/usr/lib/libc_malloc_debug.so.0 ./build/plain/prog
mtrace ./build/plain/prog build/mtrace.log
```

`No memory leaks.` is the goal. The lesson explains what mtrace can and cannot see.

## Constraints

- One allocation for the array plus one per field. No allocation inside a loop that
  is later thrown away. `strndup` is allowed (the starter defines no feature macro; add
  `#define _POSIX_C_SOURCE 200809L` before the includes).
- On failure: return NULL, leave `*count` untouched, free everything allocated in this call.
- `free_fields(NULL)` is a no-op.

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.

## Verify

`make check` passes, including the fault sweep. Every `FAIL_AT` run must exit 0 or 3 with
no LeakSanitizer report.
