# M03 e04 notes

| # | arithmetic type | value | as a malloc size |
|---|---|---|---|
| 1 | conversion to `size_t` | 18446744073709551615 (2^64 - 1) | malloc fails (ASan aborts) |
| 2 | `int` -1 converted to `size_t`, then times 4 | 18446744073709551612 (2^64 - 4) | fails |
| 3 | `unsigned` to `int`, out of range | -1294967296 | **implementation-defined**: gcc documents reduction modulo 2^32 |
| 4 | `size_t` | 2^64 - 1 | the classic `len - 1` underflow when `len == 0` |
| 5 | `uint32_t` (wraps before the cast) | 0 | **undersized allocation**: the dangerous kind |
| 6 | `size_t` (the cast binds first) | 17179869184 | correct (16 GiB) |
| 7 | `i` converted to `unsigned`: 4294967295 < 1 | 0 | n/a (a comparison bug: "-1 is not less than 1") |
| 8 | `size_t` | 2147483648 | correct. `big + 1` in `int` would be signed overflow, UB |
| 9 | `short` -1 to `unsigned` | 4294967295 | huge |
| 10 | `size_t` | 2^64 - 1 | `sizeof(x) - k` underflows silently |

gcc flagged line 2 (`-Wsign-conversion`) and line 7 (`-Wsign-compare`, which comes from
`-Wextra`). Note what it did *not* flag: line 5, the dangerous undersized product, because each step is "legal"
unsigned arithmetic. Lines with an explicit cast aren't flagged either: a cast tells the compiler you meant it.
Confirmed on the course machine (gcc 16.2.1): exactly these two warnings.
