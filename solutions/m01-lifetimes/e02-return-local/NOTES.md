# M01 e02 notes

**Mechanism.** `buf` is an automatic object in `make_greeting`'s frame. Its lifetime ends at the closing
brace. The returned value is an address into a dead frame. gcc (since 8) sees `return buf;`, warns with
`-Wreturn-local-addr`, and **compiles the return as a null pointer**, so on this toolchain the check
`g != NULL` fails (and an unchecked `printf("%s", g)` would segfault). Without that transformation the
bytes would still sit in the stack region until `clobber_stack`'s frame overwrote them with `X`.
Both outcomes, and printing the right text by luck, are consistent with undefined behaviour.

**Fix chosen: caller-provided storage.** The result is small and bounded, so the caller's stack array is the
right lifetime. It can't leak and can't fail with OOM. Alternatives:
- `static char buf[64]`: works for this harness only because `g` is never used after the second call.
  A third test holding both greetings would break. See e05.
- `malloc(len + 1)`: correct, but the caller must `free`, and the function gains an OOM path.

**Common mistake:** "fixing" it by making `buf` bigger or moving `clobber_stack`. The object is dead
whatever its size.
