# m10-e03-who-is-lost: answer key

The captures are in this directory: `capture.txt` (`make capture TOOL=asan`) and `capture-valgrind.txt`
(`make capture TOOL=valgrind`), both from the shipped starter. Nodes are 16 bytes. Names are `ant`, `bee`,
`cat`, `dog`, `eel`, `fox`, `gnu`, `hen` and `jay` (4 bytes with NUL), plus `ibis` (5).

LeakSanitizer: **97 bytes in 12 allocations.**

| Case | What leaks | Kind |
|---|---|---|
| A | head node (`cat`, 16 B) | **direct** |
| A | the other 2 nodes (32 B) and all 3 names (12 B) | **indirect** |
| B | the 4 names (16 B) | **direct**: their nodes were freed, so nothing live points to them |
| C | the unlinked `ibis` node (16 B) | **direct** |
| C | its name `ibis` (5 B) | **indirect** |

Direct: 6 objects, 48 bytes. Indirect: 6 objects, 49 bytes. LeakSanitizer groups records by allocation stack,
so A's head node and C's node appear in separate records, both from `push` line 16.

Valgrind gives the same split ("definitely lost: 48 bytes in 6 blocks", "indirectly lost: 49 bytes in 6 blocks").
Its records are grouped differently: A is one record, "60 (16 direct, 44 indirect) bytes in 1 blocks are
definitely lost", and C is "21 (16 direct, 5 indirect)".

The lesson: B is the only case where a fix inside a destroy function removes the report. A and C are pointer
bookkeeping mistakes in the caller.
