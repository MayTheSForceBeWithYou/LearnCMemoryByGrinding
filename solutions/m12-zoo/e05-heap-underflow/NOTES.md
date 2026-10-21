# m12-e05-heap-underflow: notes

**Bug.** For the all-spaces input, `p` walks down to `s - 1` and reads it: a heap underflow.
For `""` the very first `p = s + strlen(s) - 1` already points before the block.

**Mechanism.** ASan reports `heap-buffer-overflow ... located 1 bytes before 6-byte region`
at the `while (*p == ' ')` read. In the plain build, `s[-1]` is the high byte of the chunk's
size field (zero for small chunks), which is not `' '`, so the loop stopped by luck. Had
the byte been `0x20`, the loop would have *written* `'\0'` into the chunk header.

**Fix.** Index with a length that cannot go below zero:
`size_t n = strlen(s); while (n > 0 && s[n - 1] == ' ') s[--n] = '\0';`.

**Common mistake.** Adding `p >= s` to the condition. Forming `s - 1` is itself undefined
behaviour (pointer arithmetic outside the array, C17 6.5.6p8), even if you never dereference it.
