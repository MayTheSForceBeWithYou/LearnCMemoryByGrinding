# M02b e03 notes

The starter's summary, captured on the course machine (`make valgrind`, summary lines only):

```
==784241==     in use at exit: 212 bytes in 5 blocks
==784241==   total heap usage: 7 allocs, 2 frees, 4,318 bytes allocated
==784241== 80 (16 direct, 64 indirect) bytes in 1 blocks are definitely lost in loss record 4 of 5
==784241== 100 bytes in 1 blocks are definitely lost in loss record 5 of 5
==784241==    definitely lost: 116 bytes in 2 blocks
==784241==    indirectly lost: 64 bytes in 2 blocks
==784241==      possibly lost: 0 bytes in 0 blocks
==784241==    still reachable: 32 bytes in 1 blocks
==784241== ERROR SUMMARY: 4 errors from 4 contexts (suppressed: 0 from 0)
```

- **definitely lost 116 / 2:** the `struct pair` (16) and `scratch` (100). Setting `p = NULL` and `scratch = NULL` destroyed the only copies.
- **indirectly lost 64 / 2:** `left` (40) and `right` (24), reachable only through the lost pair.
- **still reachable 32 / 1:** `config_cache`. A global still points at it, so Valgrind doesn't count it as an error.
  `make valgrind` exits non-zero because of the other three categories, not this one.
- **7 allocs, 2 frees** surprises most people: the program makes 6 allocations and 1 free. The extra allocation is stdout's
  buffer, which `printf` allocates inside libc (4096 bytes, hence 4,318 total), and the extra free is libc releasing it.
  Valgrind sees *every* allocation in the process, not just yours. (`--wrap`-based fault injection, by contrast, sees only yours.)
- **4 errors:** one per loss record for definite and indirect leaks.
- **Should `config_cache` be freed anyway?** Yes. "Still reachable" at exit means "nobody freed it". In a library or a long-running
  loop, the same pattern is a real leak. Freeing it also keeps the summary at zero, so a *new* leak stands out. The solution frees it.
