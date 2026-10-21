# m08-e01-split-words: notes

**Why it is correct.** Two passes over the line: count, then copy. Counting first sizes the table
exactly, `n + 1` slots for the NULL terminator. The table comes from `calloc`, so if a `strndup`
fails halfway, every slot not yet filled is a null pointer and `free_words` is a valid cleanup for the
partial table. No second teardown routine is needed. `count_out` is written only on success.

**Alternatives.** Grow the table with `realloc` in one pass (M09 style). That's one scan instead of two, but a
`tmp` pointer and a growth policy to get right. Or copy the whole line once and point the table into
that single buffer, writing NULs over the separators: two allocations total, but then `free_words` must
know the layout, so the contract changes.

**The usual mistake.** `isspace(c)` with a plain `char`. Bytes above 127 (the harness feeds UTF-8 `é`)
are negative on x86-64, and passing a negative value other than `EOF` is undefined behaviour. Glibc's
table lookup tolerates it, so it rarely crashes. Cast to `unsigned char`. Second most common: returning
NULL for an empty line, which makes "no words" indistinguishable from "out of memory".
