# M01 e02: The returned local `[BUG]`

## Goal

`main.c` has a helper, `make_greeting(const char *name)`, that builds `"Hello, <name>!"`
in a local array and returns a pointer to it. The test harness calls it, calls one other
function (`clobber_stack`), then checks the string. The program builds with a warning
and fails its first check. Current gcc replaces the returned address with a null pointer,
so what you see is a failed check or a crash, not stale text.

Fix the defect, then write `ANSWER.md`.

## Constraints

- Keep the signature's *intent*: the caller gets a greeting string it can read after
  `make_greeting` returns. You choose the lifetime strategy (static, caller-provided
  buffer, or heap), and you may change the signature to support it. Update the call
  sites in `main` to match.
- If you choose the heap, the harness must free the result and the program must be leak-free.
- The fixed build must produce **zero warnings** with the course flags.
- `ANSWER.md` must explain, in terms of frames and object lifetimes: which object the
  original pointer referred to, when that object died, and why the failure you saw
  (a null pointer from gcc, wrong text, empty text, or a segfault) is consistent with that. Name one other outcome
  that would *also* have been consistent.

## How to verify

- `make check`: the checks, ASan, and Valgrind all clean.
- Look at the warning the original produces with `make run` before you change anything.
