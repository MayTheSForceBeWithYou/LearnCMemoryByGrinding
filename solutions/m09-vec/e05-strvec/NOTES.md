# m09-e05-strvec: notes

The contract is in this solution's `strvec.h`. Points worth defending:

- **`push_copy` grows first, then copies.** If the copy fails, the vector may have more capacity than before,
  but its contents and `len` are unchanged and nothing is orphaned. Copying first would mean freeing the copy
  when growth fails. That also works, but it's one more path.
- **A failed `push_take` leaves `s` with the caller.** The caller still holds the pointer and can retry,
  report or free it, and the function has no side effect on failure. The opposite policy ("take always
  consumes, even on failure") makes call sites one line shorter (`if (!push_take(v, s)) return OOM;` with no
  `free`). Some APIs choose it for that reason, but then the contract has to shout it.
- **`get` returns a borrowed `const char *`** that stays valid across pushes, because growth moves the table of
  pointers, not the strings. That contrasts with e04, where the elements themselves moved.
- **`pop` transfers ownership.** The caller frees.

**Grading the failure contract.** The exercise is tagged `[FAULT]`, so `make check` sweeps every allocation
(verify-all: `sweep N=57 pass=1 oom=56 fail=0`).
The harness also forces one deterministic failed take (a vector with `len == cap == SIZE_MAX / 8`, whose doubled
byte count overflows), then reads and frees the string. A `push_take` that frees `s` on failure fails that
check on the plain build, and ASan and Valgrind report the use-after-free and double free. The harness frees
`mine` itself when `push_take` fails, which is exactly what the contract says.
