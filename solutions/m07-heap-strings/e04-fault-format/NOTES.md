# M07 e04 notes

- `va_copy` before the measuring pass: `vsnprintf` consumes `ap`, and using it again is UB (on x86-64 it really does print garbage). Each
  `va_copy` needs its own `va_end`.
- `report_build` keeps the per-line strings in a zeroed array and frees `lines[0..built-1]` (that is, `i >= 0 && i < built`) at one cleanup
  label, on success and failure alike. `out` is NULL unless the final allocation succeeded, so one `return out` covers both.
- `calloc(n ? n : 1, ...)` avoids an ambiguous `calloc(0)` result for the empty report.
- A leaner alternative measures every line with `snprintf(NULL, 0, ...)`, allocates once, and formats directly, with no per-line blocks
  and only one allocation to fail. The sweep shows the difference in `N`. The reference solution sweeps at N=8 (pass=0 oom=8 fail=0, observed on course machine).
- Sweep: every `FAIL_AT` must exit 3. A leak of `lines[0]` when the third `fmt_alloc` fails is the bug this exercise exists to catch.
