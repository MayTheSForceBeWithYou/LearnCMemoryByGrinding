# M07 e01 notes

- `my_strdup`: one `strlen`, then `malloc(len + 1)` and `memcpy(len + 1)`, which copies the NUL along with the text.
- `my_strndup`: the length is `min(strlen(s), n)`, but `strlen(s)` would scan past `n` on an unterminated buffer. The harness's
  4-byte block with no NUL makes that a `heap-buffer-overflow` under ASan. The bounded loop is exactly `strnlen` (POSIX), which you
  could use with the feature-test macro defined.
- The result size is the *result's* length + 1, never `n + 1`. `strndup("a", 100)` allocates 2 bytes, not 101.
- Common mistake: `memcpy(d, s, n)` "because n is the max". It reads past the source's NUL into whatever follows.
