# m12-e02-double-free: notes

**Bug.** `doc_copy` does `*dst = *src`: a shallow copy. Both `Doc`s then point to the same
`title` block, and each `doc_fini` frees it.

**Mechanism.** Plain build: the first `free` puts the chunk into the tcache and writes the
tcache key into it. The second `free` sees the key, finds the chunk in its bin and aborts:
`free(): double free detected in tcache 2`. ASan: `attempting double-free`, with the freed-by
stack at the first `doc_fini(&a)` and allocated-by at `doc_init`'s `strdup`, on the `15-byte region` that holds `"Notes on Heaps"`.

**Fix.** Deep copy: after `*dst = *src`, `dst->title = strdup(src->title)` and return -1 if
it is NULL. Nothing else was allocated, so nothing leaks on failure. (`*dst` must not be
`doc_fini`'d after a failed copy. `main` already follows that.)

**Common mistake.** "Fixing" it by removing one `doc_fini`. That makes the copy's lifetime
depend on the original's, which is exactly the hidden sharing the contract forbids.
