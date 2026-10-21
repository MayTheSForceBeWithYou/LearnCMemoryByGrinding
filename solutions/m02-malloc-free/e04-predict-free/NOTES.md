# M02 e04 notes

Captured on the course machine (Arch, glibc, plain build `./build/plain/prog N`, stderr), plus ASan's headline from
`./build/asan/prog N`:

| # | outcome (plain) | glibc message | exit | ASan headline |
|---|---|---|---|---|
| 1 | defined, no output | none | 0 | none |
| 2 | defined (`free(NULL)`) | none | 0 | none |
| 3 | glibc abort | `free(): double free detected in tcache 2` | 134 (SIGABRT) | `attempting double-free` |
| 4 | glibc abort | `free(): double free detected in tcache 2` | 134 | `attempting double-free` |
| 5 | glibc abort | `double free or corruption (out)` | 134 | `attempting free on address which was not malloc()-ed` |
| 6 | glibc abort | `free(): invalid pointer` | 134 | `attempting free on address which was not malloc()-ed` |

Why each:
- **3.** The tcache stores a per-chunk "key" in the freed chunk. A second free of a chunk whose key matches is caught cheaply.
- **4.** `q = malloc(...)` gets the same chunk back (same size class, LIFO tcache), so `free(p)` frees *q's* block,
  which is legal as far as glibc can tell. The *following* `free(q)` is the one that aborts. The defect is on the line
  `free(hide(p))`, but the abort is one line later. Under ASan the quarantine stops the chunk from being recycled, so ASan
  reports exactly `free(p)`.
- **5.** The stack array has no chunk header. glibc reads garbage as a size, and its sanity checks fail. The message wording
  ("double free or corruption") is misleading: no double free happened.
- **6.** `p + 1` is misaligned for a chunk (not 16-byte aligned), which is the cheapest check of all.
- "Silent UB" didn't occur here, but don't generalise. Variants of 4 with different sizes, or 5 with a stack layout that
  happens to look like a valid header, can run "successfully". A glibc abort is a courtesy, not a guarantee.
- gcc 12+ warns `-Wuse-after-free` on scenarios 3 and 4 even through `hide()`, because it tracks the variable `p`, not the call.
