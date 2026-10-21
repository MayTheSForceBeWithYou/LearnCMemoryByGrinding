# m13-e03-asan-stop `[FAULT]`

## Goal

Implement `parse_ints()` and `intlist_free()` as specified in `intlist.h`: parse
`"4,-2,17"` into a growable heap array, reject malformed input exactly, and survive an
allocation failure at every allocation point. Write them in `main.c`.

This exercise is about **debugging sweep failures with gdb**. When `make fault` prints a
failing `FAIL_AT=k`, reproduce that single run under gdb:

```
make build-fault
FAIL_AT=k gdb -q build/fault/prog
(gdb) run
(gdb) bt
```

- A `SIGSEGV` (writing through a NULL you did not check) stops in gdb *before* ASan's SEGV
  handler runs: `bt`, `frame 1`, `info locals`.
- For an ASan access report, the report's first line names the access (`READ of size 4` →
  `__asan_report_load4`). `break __asan_report_load4` before `run` stops at the bad access
  with the process alive. Without it, `abort_on_error=1` turns the report into `SIGABRT`,
  and `bt` still shows your frame.
- A LeakSanitizer report cannot be reproduced under gdb (LSan does not run under ptrace).
  Read its allocation stack, and ask which error path forgot that block.

Record in `NOTES.md` (yours) one sweep failure you hit, and the gdb session that explained it.

## Constraints

- Use `strtol` with `errno` and `end`-pointer checks. `atoi` cannot report errors.
- Growth by doubling, so the number of `realloc` calls depends on the input. The sweep finds
  them all.
- On `-1` and `-2`, `*out` is untouched and nothing leaks.

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.

## Verify

`make check` passes, including the fault sweep: every `FAIL_AT` exits 0 or 3, with no report.
