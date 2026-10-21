# M07 e02 notes

- For `"[Lv 7] Ada!"`: the length is 4 + 1 + 2 + 3 + 1 = 11, so the size needed is 12. The code allocated `3 + 1 + 7 = 11` bytes. `sprintf` writes the NUL
  at index 11, which is "0 bytes after 11-byte region". The comment listed five parts, and the constant counted four of them: the `"!"` was added to the format later.
- **Plain build:** glibc rounds every request up to a chunk of at least 24 usable bytes, so index 11 lands in padding that belongs to the same chunk.
  No crash, no corruption you can see.
- **Valgrind:** it does track the exact requested size, so `make valgrind` alone would report "Invalid write of size 1". In `make check`, ASan
  runs first and fails first.
- **Fix:** `+ 8`. A better fix derives the size from the format itself: `snprintf(NULL, 0, "[Lv %d] %s!", level, name) + 1`. Then
  `digits()` and the hand count both disappear, and they can never drift again.
