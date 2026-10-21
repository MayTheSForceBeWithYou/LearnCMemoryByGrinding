# M04 e03 notes

- **TRACE 1:** B1 is 12 bytes and live, with defined bytes `i >= 0 && i < 12` ("hello world" + NUL). `b.data` -> B1. `saved_ptr` -> B1 + 6.
  `b.data + saved_off` -> B1 + 6.
- **TRACE 2, in place:** B1 is now 24 bytes at the same address, and `saved_ptr` happens to be valid. Defined bytes are `i >= 0 && i < 12`,
  and `i >= 12 && i < 24` is the uninitialised grown tail.
  **Moved:** B1 is freed, and B2 (24 bytes) holds a copy of the 12 bytes, with the tail `i >= 12 && i < 24` uninitialised. `saved_ptr` dangles into B1.
- **TRACE 3:** a 1 MiB request is above glibc's mmap threshold (128 KiB by default), so it's almost certainly a new `mmap`ed block B3.
  The previous block is freed. Defined bytes are `i >= 0 && i < 12`. Everything else is uninitialised. (Bytes 12..23 were never written in any branch.)
- **Observed on the course machine:** the plain build grew in place to 24 and moved for 1 MiB. **The ASan build moves on every
  realloc**, because ASan's allocator never grows in place and quarantines the old block, which is exactly so that stale
  interior pointers fault. Which branch you're in depends on the allocator, so code must assume "moved".
- Through `saved_ptr`, any read or write after either grow is undefined in the moved branch. In the in-place branch, it's defined only
  because of luck nothing in the source can promise.
