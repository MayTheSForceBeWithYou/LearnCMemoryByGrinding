# m12-e13-which-tool: notes

The real matrix is in `capture.txt` beside this file. It was captured from the shipped source by `make capture TOOL=matrix`.

| | plain | ASan+UBSan | Valgrind |
|---|---|---|---|
| `nul` | exit 0 | ASan `heap-buffer-overflow`, abort (134) | `Invalid write of size 1`, exit 1 |
| `uninit` | prints `not positive`, exit 0 | **nothing reported**, exit 0 | `Conditional jump or move depends on uninitialised value(s)`, exit 1 |
| `stack` | exit 0 | UBSan `index 4 out of bounds for type 'int [4]'`, exit 1 | **nothing reported**, exit 0 |

**`nul`.** `malloc(5)` receives 6 bytes. Plain glibc has 24 usable bytes, so nothing is
noticed. ASan's redzone starts at byte 5, and so does Memcheck's.

**`uninit`.** ASan does not track initialisation. Its `0xbe` fill makes `v[3]` equal to
`0xbebebebe`, which is negative as an `int`. So the ASan build also prints `not positive`, but
for a different reason than plain (zeroed fresh memory). Memcheck reports the `if`, which is
the first point where the undefined value decides control flow.

**`stack`.** `n = argc + 2 = 4`, so the loop stores `a[4]`. Memcheck cannot see it: it has no
redzones between stack variables, and `a[4]` is just another valid word of the stack frame.
In the sanitizer build, `-fsanitize=bounds` (part of `undefined`) checks the index *before*
the store. The course builds with `-fno-sanitize-recover=all`, so the run ends right there
with exit 1, before the store executes.

**Lesson.** No single tool covers the matrix. Run both, every time, as `make check` does.
