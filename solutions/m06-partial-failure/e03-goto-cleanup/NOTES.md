# M06 e03 notes

- Five resources, one exit. Every local starts NULL, so `cleanup:` releases unconditionally. `free(NULL)` is a no-op, and the
  `FILE *` needs its own `if`, because `fclose(NULL)` is undefined.
- **Transfer by NULLing:** `s->title = title; title = NULL;` moves ownership into the struct, so cleanup won't free it, and
  `*out = s; s = NULL;` moves the struct to the caller. Success runs the same cleanup code and releases only the line buffer and the file.
- `*out` is written last, only on success. The missing-file and malformed paths never touch it.
- Sweep on the course machine: `N=8 pass=3 oom=5`. The five OOM runs fail, in turn, the line buffer, the title `strdup`, each grow of `vals` (4 to 8, so 2 reallocs for 8 values), and the
  result struct, in the scores.txt load. The missing-file load allocates nothing, and the bad.txt load allocates line, title, and one
  `vals` array before failing on "twenty". Failing one of those three just makes that load return -1 earlier, which is what the check expects, so those 3 runs pass. `fopen`'s own allocation is invisible to `--wrap`. The missing-file test covers the `fopen` failure path instead.
- `fgets` into our own buffer instead of `getline`: `getline` allocates inside libc, so the sweep could never fail it.
- Common bug: `return -1;` in the middle of the loop, which leaks the file, line, title, and vals at once. The sweep finds it on the first failure point.
