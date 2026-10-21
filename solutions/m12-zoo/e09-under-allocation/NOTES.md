# m12-e09-under-allocation: notes

**Bug.** `malloc(n * sizeof pts)`: `sizeof pts` is the size of a *pointer* (8), not of a
`Point` (16). Six points need 96 bytes, and the block has 48.

**Mechanism.** ASan: `heap-buffer-overflow` on the store to element 3 (offset 48),
`0 bytes after 48-byte region`, allocated in `make_points`. The accessed-at line is the
symptom. The cause is the allocated-by line's size expression. Plain build: the writes run
48 bytes past the chunk into the top chunk's header and overwrite its size field with
double bit patterns. The next allocation served from top fails glibc's top-size check:
`malloc(): corrupted top size`. Memcheck reports the invalid writes at lines 17 and 18,
and then **Valgrind itself dies**: `Assertion 'bszB_lo == bszB_hi' failed ... Heap block
lo/hi size mismatch`. The writes went past Memcheck's redzone into its own heap-block
metadata. Valgrind's message tells you to fix the first invalid write it reported, and that
is the right reading: trust the first report, not what follows it.

**Fix.** `sizeof *pts`. The idiom `p = malloc(n * sizeof *p)` cannot be wrong this way,
because it names the pointee of the very variable being assigned.
