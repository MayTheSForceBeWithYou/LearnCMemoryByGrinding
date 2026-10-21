# m12-e03-uaf: notes

**Bug.** `before` borrows the old value's block. The second `settings_set` frees that block
(documented), and `strcmp(before, "dark")` then reads freed memory.

**Mechanism.** ASan keeps the block in quarantine, shadow `fd`, and reports
`heap-use-after-free` with freed-by at `settings_set`'s `free(e->value)` and allocated-by at
the first `strdup`. In the plain build, glibc put the 5-byte chunk into the tcache. Its first
8 bytes now hold the (mangled) tcache next pointer and the next 8 the key, so `before` reads
garbage: the check fails without a crash.

**Fix.** Keep what you need, not a pointer: `char was = settings_get(&s, "theme")[0];`
before the second set, and check `was == 'd'`. A full `strdup` copy is also correct, but
adds an allocation and an OOM path for no reason here.
