# m08-e06-copy-rows: notes

**Why it is correct.** The table comes from `malloc`, so unfilled slots are garbage. The failure path frees
exactly `rows[0..i-1]` by counting down from the failed index, then frees the table. The terminator is
written only after every copy succeeded. `(n + 1) * sizeof` is guarded against overflow.

**Sweep.** The harness makes 7 allocation calls: table for `n == 0`, table for `n == 5`, and five `strdup`.
`FAIL_AT=1` and `FAIL_AT=2` fail a table, and `FAIL_AT=3..7` fail a row at every position, including the
last. Each run exits 3 with nothing leaked. The real numbers are in the verify-all row (`sweep N=7 pass=0 oom=7`).

**Alternative.** `calloc` the table and reuse `free_rows` as the failure path. That works because the slots
past the failure are null. It's one teardown routine instead of two, which is the design the lesson's worked example
discusses.

**The usual mistake.** `for (size_t j = 0; j <= i; j++) free(rows[j]);` frees `rows[i]` (NULL here, so
harmless). The off-by-one people actually ship is `for (j = 0; j < n; j++)`. That frees garbage slots past `i`,
which `FAIL_AT=3` exposes on the first run.
