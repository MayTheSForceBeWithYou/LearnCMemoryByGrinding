# m09-e04-held-pointer: answer key

From the capture (`capture.txt` in the exercise):

1. **Bad access:** `main.c:40`, a READ of size 4. That's `scores[i] > *best` in the `if` condition.
2. **Freed by:** the `realloc` in `vec_push` at `main.c:17`, called from `main.c:35`. Growing from 4 to 8
   ints (the 5th push) gave the vector a new block and freed the old one, which `best` still pointed into.
3. **Allocated by:** the same `realloc` line, on its first call (`realloc(NULL, ...)`, acting as `malloc`).
   The "freed" and "allocated" stacks are identical, which is the signature of a block replaced by `realloc`.
4. **Why the plain build passed on the course machine:** glibc resized the block in place, so `best` still pointed
   at live memory with the right value. That isn't guaranteed.
5. **Fix:** remember the *index* of the best element, not its address. Indices survive growth. (Alternatives:
   reserve all capacity up front and document that pointers stay valid, or keep the best *value*.)
