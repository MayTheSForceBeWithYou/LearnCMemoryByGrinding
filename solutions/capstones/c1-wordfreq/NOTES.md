# c1-wordfreq notes

- **Table:** open addressing with linear probing, FNV-1a hashing, power-of-two capacity, and load factor kept at 1/2 or below. There's
  no deletion, so no tombstones are needed: an empty slot (`word == NULL`) ends every probe.
- **Equality** without reading past `len`: `strncmp(slot, word, len) == 0 && slot[len] == '\0'`. The stored word is NUL-terminated, so
  `slot[len]` is in bounds whenever the first `len` bytes matched.
- **Failure atomicity in `wf_add`:**
  1. Look up the word first; a hit needs no allocation.
  2. Copy the word.
  3. Grow if needed. `grow` builds a complete new slot array and frees the old one only on success, and on failure `wf_add` frees the
     copy.
  4. Probe again, because indices from the old capacity are meaningless.
- **Rehash** moves `char *` pointers and never re-copies strings. That makes ownership transfer in bulk: each word is owned by exactly
  one slot at every moment.
- **`wf_read`** owns a growable scratch buffer, freed on every exit. A word straddling a failure is simply not counted, and the contract
  says words already counted stay counted.
- **`wf_sorted`** returns borrowed `word` pointers. In this implementation they survive even a rehash, since rehash moves slots,
  not strings. The contract still promises only "until the next wf_add", which leaves the implementation free to change.
- `wf_read` checks `ferror` after the loop, because `fgetc`'s `EOF` covers both end of file and a read error. A read error returns -2,
  and `main` exits 2 (an I/O failure, not OOM).
- `getline` is avoided by spec. `_POSIX_C_SOURCE` is therefore unnecessary here, and none is defined.
