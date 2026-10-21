# m08-e03-lost-table: answer key

Captured with `make asan` and `make valgrind` on the shipped `main.c` (gcc 16.2.1, glibc 2.44,
Valgrind 3.25.1, Arch WSL2). Byte counts follow from the source: pointers are 8 bytes, and the strings
are `north` (6 with NUL), `south` (6), `east` (5) and `west` (5).

LeakSanitizer summary: **103 bytes in 9 allocations**, four records:

| Case | Record | Kind | Line |
|---|---|---|---|
| A | 32 bytes in 1 object (the 4-slot table) | **Direct** | `make_rows` line 14 (`calloc`) via `case_a` |
| A | 22 bytes in 4 objects (the rows) | **Indirect** | `make_rows` line 18 (`strdup`) via `case_a` |
| B | 32 bytes in 1 object | **Direct** | `case_b` line 31 |
| C | 17 bytes in 3 objects (`north`, `south`, `east`) | **Direct** | `make_rows` line 18 via `case_c` |

So that's 5 direct objects (81 bytes) and 4 indirect (22 bytes).

**Case C is the interesting one.** Once the table is freed, nothing live points to the rows. The bytes
of the freed table aren't scanned, so the rows are *direct* leaks. Case A's rows are indirect only because the
leaked table still exists and still points to them.

Valgrind agrees with that split: definitely lost 81 bytes in 5 blocks, indirectly lost 22 bytes in
4 blocks. It also folds A into one record, "54 (32 direct, 22 indirect) bytes in 1 blocks are definitely
lost".

**One `free` makes a case vanish:** only B (`free(g)` before `g = NULL`). A needs the rows freed and then
the table (`free(t)` alone would turn the 4 indirect leaks into 4 direct ones, which is exactly case C).
C needs a loop over the rows before `free(t)`.
