# M06 e02: The leak on step three `[BUG]` `[FAULT]`

## Goal

`session.c` builds a `struct session` in four allocation steps: the struct, a copy of the user
name, an I/O buffer, and a zeroed history array. Normal runs, ASan, and Valgrind are all clean.
The fault sweep isn't: at exactly one `FAIL_AT` value per session, LeakSanitizer reports a leak.

Find the defect from the sweep output, fix it, and write `ANSWER.md`.

## Constraints

- `ANSWER.md`: the failing `FAIL_AT` value(s), the leaked block (size and allocation site from the
  report), and the failure table for `session_create`: one row per step, with what's acquired and
  what must be released.
- Exactly one defect was planted. Fix it without restructuring the function: the existing cleanup
  style stays.

## How to verify

- `make fault` shows which `FAIL_AT` fails. After the fix, the whole sweep is clean.
- `make check` is clean.
