# Capstone c1: wordfreq `[FAULT]`

## Goal

Build a word-frequency counter on a hash table that you write yourself and that **owns every string in it**. `make run` reads
`sample.txt` and prints the five most frequent words:

```
      7 the
      3 ask
      3 dog
      3 fox
      2 don't
```

The full contract is in `wf.h`; implement every function in `wf.c`. `main.c` drives it: first a self-test that forces several rehashes,
then the sample file.

**The starter will not build until you write the definitions.** The link error ("undefined reference to `wf_new`", ...) is
intentional.

This project uses all of M01-M07:
- owned versus borrowed strings (M05, M07): `wf_entry.word` is borrowed, and `wf_free` frees every copy;
- growth without losing data on failure (M04): a rehash that fails must leave the old table intact;
- `calloc` for zeroed slot arrays, plus overflow-checked sizes (M03);
- single-exit cleanup on every failure path (M06).

## Constraints

- A word is a maximal run of ASCII letters and apostrophes, lower-cased. Read with `fgetc` (or `fread`) into a buffer you grow. Don't use
  `getline`. The fault injector works by linking with `--wrap=malloc` and friends, which only redirects
  calls made from *your* object files. `getline` allocates from inside libc, so the sweep would never
  fail those allocations and that failure path would go untested.
- `wf_add` takes a length and must not read `word[len]`. The self-test passes `"applesauce"` with `len = 5`.
- If `wf_add` fails, the table is **unchanged and still usable**. This includes a failure inside the rehash.
- `fgetc` returns `EOF` both at end of file and on a read error. Tell them apart with `ferror`, and
  report a read error as `wf.h` specifies.
- No fixed limits: no maximum word length, no maximum table size.
- On any allocation failure the program exits `EXIT_OOM` (3) with nothing leaked. `main.c` already does this, provided your functions
  report failure honestly and clean up after themselves.

## How to verify

- `make run`: prints the table above, then `c1-wordfreq: all checks passed`.
- `make check`: checks, ASan, Valgrind, then the fault sweep, which fails each allocation in turn. Every run must end in pass or exit 3.
  Expect a few hundred runs; the self-test alone adds 2003 words, 502 of them distinct.

<details><summary>Hint</summary>

Ask of every allocation in `wf_add`: if *this* one fails, is the table exactly as it was before
the call?

</details>

Cross-reference: RPG ch10 (`map_create`/`map_destroy`) and appendix C (allocation failure).
