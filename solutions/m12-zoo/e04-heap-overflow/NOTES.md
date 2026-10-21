# m12-e04-heap-overflow: notes

**Bug.** `total` counts the words and the NUL but not the `n - 1` separators. The words
are 16 bytes, so the block is 17. The string needs 16 + 3 * 3 + 1 = 26, so 9 bytes are
written past the end. The first one is written while copying `"share"` (offsets 14-18).

**Mechanism.** ASan: `WRITE of size 5`, `0 bytes after 17-byte region`, at `main.c:25`
(the word `memcpy`). It reports the first bad byte of the range. Memcheck reports three
invalid writes (word copy, separator copy, NUL) and then an invalid read in `strcmp`.
Plain glibc: a 17-byte request gets 24 usable bytes, and bytes 24-25 (`'c'` and the NUL)
land on the low two bytes of the next chunk's size field. Nothing checked that field
before exit, so the run "passed". That is luck, not correctness.

**Fix.** `if (n > 1) total += (n - 1) * strlen(sep);`. For `n == 0` the result is `""` (1 byte).

**Overflow note.** With adversarial input `total` could wrap. The course's size-arithmetic
rule (M03) would add checks. They are not required for this exercise's contract, and
adding them is not wrong.
