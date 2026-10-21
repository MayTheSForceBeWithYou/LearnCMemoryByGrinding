# m12-e14-strv-dup: notes

Count, allocate `n + 1` pointers, `strdup` each, and on failure at `i` free `i-1 .. 0`
and the array. The harness makes 6 allocations (5 + 1), so the sweep runs `FAIL_AT=1..6`.

**Usual mistakes:** calling `strv_free(out)` on the partial array. That loops until a NULL
that has not been written yet: a read of uninitialised memory, which the sweep catches only
indirectly. Or allocating `n` slots and forgetting the terminator.
