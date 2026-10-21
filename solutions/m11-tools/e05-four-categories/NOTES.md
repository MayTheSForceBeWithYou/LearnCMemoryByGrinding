# m11-e05-four-categories: notes

`capture.txt` beside this file is the real `make valgrind-all` output for the shipped source.

| block | size | memcheck | why |
|---|---|---|---|
| 1 `cache` | 100 | still reachable | a global (a root) points at its start |
| 2 `scratch` | 40 | definitely lost | the only pointer was set to NULL |
| 3 `box` | 8 | definitely lost | same: `32 (8 direct, 24 indirect)` in one record |
| `box->payload` | 24 | indirectly lost | its only pointer lives inside lost block 3 |
| 4 `ring` | 64 | possibly lost | only the global `cursor` points to it, and into its middle (`ring + 10`) |

`LEAK SUMMARY`: definitely lost 48 bytes in 2 blocks, indirectly lost 24 in 1, possibly
lost 64 in 1, still reachable 100 in 1.

**`make valgrind`** (course flags) shows the definite, indirect and possible records, hides
the reachable one, and prints `ERROR SUMMARY: 4 errors from 4 contexts`. The four errors are
the loss records 1-4: still reachable is not in `--errors-for-leak-kinds`. Valgrind exits 1
(`--error-exitcode=1`), so make fails.

**`make asan`** (LeakSanitizer) reports `Direct leak of 40`, `Direct leak of 8` and
`Indirect leak of 24`: 72 bytes in 3 allocations. It does **not** report `ring`: LSan
counts a pointer into the interior of a block as a reference, so `cursor` keeps it alive.
It does not report `cache`, which is reachable. LSan has no "possibly lost" category, so
the same program gives memcheck four errors and LSan three leaks.

**Lesson.** "Possibly lost" is memcheck being honest that an interior pointer may or may not
be deliberate. Treat it as a leak until you can name the pointer arithmetic that gets back
to the start, then free it explicitly anyway.
