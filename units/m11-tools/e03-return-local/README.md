# m11-e03-return-local `[REPORT]`

## Goal

`capture.txt` is a real ASan report from this `main.c`, captured with
`ASAN_OPTIONS=...:detect_stack_use_after_return=1` (the Makefile adds it via `ASAN_EXTRA`).
Write `ANSWER.md`:

1. What the shadow value `f5` means, and which frame's memory is being read.
2. Why `READ of size 7` and not 1 (look at what `check` calls), and why frame `#0` has no name.
3. Why `-Wall -Wextra` gave no warning here, although M01 showed GCC warning about returning a local's address.
4. What ASan reports with the check switched **off**, and why. Predict it, then try it:
   `ASAN_OPTIONS=detect_stack_use_after_return=0 ./build/asan/prog`. Also find out what this
   libasan's default is: `ASAN_OPTIONS=help=1 ./build/asan/prog 2>&1 | grep -A1 use_after_return`.

Then change the API so the caller owns the buffer, and fix `main`.

## Constraints

- `format_label` stays as it is. The label must still be built by `label_for`.
- No heap allocation and no `static` buffer: the caller provides storage.

## Verify

`make check` passes (it runs ASan with the option on).
