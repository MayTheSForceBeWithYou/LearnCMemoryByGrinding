# m11-e03-return-local: notes

1. `f5` is "Stack after return": the fake frame of `label_for` (line 15), poisoned when it
   returned. The access is inside `'buf' (line 16)`, at offset 32 of that frame.
2. `strcmp` reads `"item-7"` plus its NUL, 7 bytes, in one intercepted check. Frame `#0` is
   ASan's `strcmp` interceptor inside `libasan.so`, which has no symbols on this machine.
3. `-Wreturn-local-addr` sees only a function returning the address of its own local.
   Here `label_for` returns whatever `format_label` returned. At `-O0` GCC does not track
   that the value is `buf`.
4. Nothing: with `=0` the run prints `all checks passed` (checked on the course machine).
   With the check off, locals live on the real stack. After return, that memory is still
   addressable stack (shadow `00`), so the read is legal as far as ASan is concerned, and
   nothing has overwritten the bytes yet. `help=1` shows `(Current Value: true)` with no
   option set: this libasan has the check on by default. The Makefile's `ASAN_EXTRA` makes
   the exercise independent of that default.

**Fix.** `label_for(char *buf, size_t size, int id)` forwards the caller's buffer, and
`main` declares `char buf[16]`. Lifetime now matches use.
