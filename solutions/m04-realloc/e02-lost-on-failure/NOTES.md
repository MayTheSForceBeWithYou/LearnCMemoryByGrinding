# M04 e02 notes

The words total 47 characters plus 10 spaces, so 57 per round and 342 bytes (343 with the NUL). Growth goes 16, 32, 64, 128, 256, 512: six `realloc` calls, so N = 6.

**What the sweep shows, as shipped** (observed on course machine): `N=6 pass=0 oom=0 fail=6`.
- `FAIL_AT=1`: UBSan, `strbuf.c:22:5: runtime error: null pointer passed as argument 1, which is declared to never be null` (exit 1).
- `FAIL_AT=2..6`: ASan SEGV, a WRITE near address 0 (exit 134).

No leak report and no heap overflow appear while both defects are present.

**Defect 1, `sb->data = realloc(sb->data, newcap)`.** On a failed grow, the old block is still alive, but its only pointer is overwritten
with NULL. That block leaks (16, 32, ... bytes). LSan would report it at exit, but the run never gets there: `main`'s recovery
append writes through the NULL `data` first. That is the SEGV above.

**Defect 2, `sb->cap = newcap` before the call.** After a failure, `cap` claims `newcap` bytes. The recovery append `"!"` then skips
the grow (it "fits") and writes at `data + len`:
- with defect 1 present, `data` is NULL: a write near address 0 (UBSan catches the `memcpy` NULL argument at `FAIL_AT=1`, where `len` is 0);
- with defect 1 fixed, past the end of the *old* block. Observed with only defect 1 fixed: `N=6 pass=0 oom=3 fail=3`. `FAIL_AT=3` gives
  `heap-buffer-overflow ... 0 bytes after 32-byte region`, and `FAIL_AT=4` gives `0 bytes after 64-byte region`, both in `memcpy`.
  `FAIL_AT=1` is still the NULL argument. At 2, 5 and 6 the recovery `"!"` happens to fit in the old block, and the run exits 3.

**Fix:** realloc into `tmp`, check, then commit `data` and `cap` together. After that, every `FAIL_AT` exits 3 cleanly: the recovery append
succeeds, because the injector fails only one call.

The lesson: failure-path bugs are invisible to a normal test run. `make check` without the sweep passes on the buggy starter.
