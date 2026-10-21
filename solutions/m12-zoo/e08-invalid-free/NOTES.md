# m12-e08-invalid-free: notes

**Bug.** The scan advances `s` itself, so `free(s)` passes a pointer to the copy's
terminating NUL (byte 23 of the 24-byte block), not the pointer `strdup` returned.

**Mechanism.** `free` must receive exactly a value returned by an allocation function
(C17 7.22.3.3). glibc reads the "chunk header" just before the pointer, finds nonsense and
aborts with `free(): invalid pointer`. ASan reports `attempting free on address which was not
malloc()-ed`, the address being inside the 24-byte region.

**Fix.** Keep the owner pointer separate from the cursor: `char *copy = strdup(line); char *s = copy;`
and `free(copy)`.
