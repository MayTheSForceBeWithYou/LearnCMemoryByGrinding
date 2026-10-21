# m11-e04-direct-indirect: notes

The real report is in `capture.txt` (captured from the shipped starter source):

| Record | Bytes | Objects | Line | Why |
|---|---|---|---|---|
| Direct | 80 | 1 | 59 | `table[1]`, 20 ints; its only pointer was in the freed table |
| Direct | 40 | 1 | 58 | `table[0]`, 10 ints; same reason |
| Direct | 16 | 1 | 16 (via `push`, line 44) | the head node "cy"; nothing points to it |
| Indirect | 32 | 2 | 16 | nodes "bob" and "ann", reachable only from the leaked head |
| Indirect | 11 | 3 | 19 (`strdup`) | "cy\0" 3 + "bob\0" 4 + "ann\0" 4 |

`SUMMARY: AddressSanitizer: 179 byte(s) leaked in 8 allocation(s).`

**Plausible wrong reading:** "the rows of B are indirect, because a table pointed to them."
LSan classifies by the state at exit. Once the table is freed, its memory is neither a root
nor a leaked block, so nothing points to the rows and they are direct. Freeing a container
before its contents does not shrink the leak. It promotes the contents to direct leaks.

Records with identical allocation stacks are merged ("in 2 object(s)"), which is why the
two non-head nodes share one record. The head node has its own record only because LSan
classifies it differently (direct), not because its stack differs.
