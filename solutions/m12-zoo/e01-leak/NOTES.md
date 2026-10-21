# m12-e01-leak: notes

**Bug.** When `w` equals the target, the loop does `hits++; continue;`, skipping `free(w)`.
Only matching words leak: 3 matches of 6 bytes ("apple\0") = `18 byte(s) leaked in 3 allocation(s)`.

**Fix.** Free on every path: `if (strcmp(w, t) == 0) hits++; free(w);`.

**Mechanism.** Each `normalize` result is the only pointer to its block. Leaving the loop
iteration drops it. LSan finds no reference from any root at exit, so it reports a direct leak
at `normalize`'s `malloc`, called from `count_equal`.

**Common mistake.** Adding `free(w)` before `continue` *and* keeping the one after works,
but duplicates cleanup. A `continue` that skips cleanup is the pattern to unlearn. Write the
loop body so there is one exit.
