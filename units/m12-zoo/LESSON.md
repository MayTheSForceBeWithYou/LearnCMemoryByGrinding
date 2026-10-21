# M12: Bug hunt: the heap bug zoo

## Organizer

M11 taught what each tool measures. This unit is practice at the other end: you get a
program that misbehaves, you identify the bug class from the evidence, and you fix the
cause without touching anything else. There is one exercise per class, then two mixed-bag
files with no hint about which classes they contain.

You need M02-M04 (allocation, `calloc`, `realloc`), M07 (heap strings) and M11 (reading
reports). Each `[BUG]` exercise asks for `ANSWER.md`: what was wrong, the mechanism (what
the machine actually did), and how the tool showed it.

## Foundations

### The classes, by mechanism

| Class | What happens in memory | Who notices first |
|---|---|---|
| leak | a block's last pointer is lost or overwritten | LSan at exit; memcheck "definitely lost" |
| double free | a block is returned to the allocator twice | ASan `attempting double-free`; glibc tcache check |
| use after free | a read/write through a pointer into a freed block | ASan `heap-use-after-free` (while in quarantine) |
| heap overflow | access at or past `base + size` | ASan `heap-buffer-overflow ... after`; memcheck `Invalid write` |
| heap underflow | access before `base` | ASan `... before N-byte region`; memcheck `is N bytes before` |
| off-by-one NUL | the terminator written at `base[size]` | ASan and memcheck; glibc slack hides it in plain runs |
| uninitialised read | a decision depends on bytes never written | **memcheck only** |
| invalid free | `free` of a pointer `malloc` never returned (interior, stack, static) | ASan; glibc `free(): invalid pointer` |
| under-allocation | `malloc(n * sizeof p)` where `sizeof *p` was meant | shows up as a heap overflow; the cause is at the `malloc` |

Two of these are *causes* that present as another class. An under-allocation is diagnosed
as an overflow, and you must walk back from the accessed-at stack to the allocated-by stack
and compare the requested size with what the code uses. An off-by-one NUL is an overflow of
exactly one byte at exactly `size`, almost always from a length computed without the `+ 1`.

### Slack: why plain runs lie

glibc's `malloc(n)` returns a chunk with at least `n` usable bytes, rounded up: the
minimum chunk on x86-64 is 32 bytes (24 usable), and sizes go in steps of 16. A one-byte
overflow of `malloc(8)` lands in bytes the chunk already owns. Nothing breaks, and nothing
is detected, until a size change moves the boundary. ASan poisons the slack byte-exactly,
and Valgrind's own allocator puts a redzone right after the requested size.

### glibc's own checks, with no sanitizer present

Without ASan, glibc still validates some invariants on every `malloc`/`free` and aborts
with a message on `stderr` and `SIGABRT`. Two you will see:

- `free(): double free detected in tcache 2`. Each thread caches small freed chunks
  (by default requests up to 1032 bytes, 7 chunks per size class) in a *tcache*. A freed chunk stores a
  per-process random `key` in its second word. `free` checks that word, and if it matches,
  scans that size's tcache list for the same chunk. A repeated `free` of the same pointer
  with nothing in between is caught every time.
- `malloc(): corrupted top size`. The *top chunk* is the free space at the end of the
  heap, and its size field sits directly after the last chunk carved from it. When a
  request is served from top, glibc checks that the size is plausible (no larger than the
  memory the arena got from the kernel). An overflow from the last block that rewrites
  that field with text or garbage trips the check on the next large-enough request, which
  may be in a completely different function, or inside `printf` allocating stdout's buffer.
  Which block borders top depends on history. glibc allocates the tcache's own bookkeeping
  block lazily, so the same overflow off an early small block can land in *that* block's
  header instead, and nothing checks it. M12 e10 had to be arranged around exactly this.

From `lesson-demo/` (`capture-tcache.txt`):

```
free(): double free detected in tcache 2
exit 134
```

134 is 128 + 6: killed by signal 6, `SIGABRT`. These checks are heuristics. A double free whose
chunk was handed out again in between, or a corruption that writes a *plausible* size,
passes silently. Treat a glibc abort as a gift, and never treat its absence as evidence.

### The same bug through two tools

`lesson-demo` reads one `int` past a 4-`int` block:

```
==797970==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x6eed8a6e0020 ...
READ of size 4 at 0x6eed8a6e0020 thread T0
    #0 0x57dcc958c3f9 in read_past .../lesson-demo/main.c:16
0x6eed8a6e0020 is located 0 bytes after 16-byte region [0x6eed8a6e0010,0x6eed8a6e0020)
```

```
==797997== Invalid read of size 4
==797997==    at 0x40011F0: read_past (main.c:16)
==797997==  Address 0x4aa3050 is 0 bytes after a block of size 16 alloc'd
==797997==    at 0x48668A8: malloc (vg_replace_malloc.c:446)
==797997==    by 0x400118A: read_past (main.c:9)
```

The two tools agree on everything that matters: the access size, the offset relative to
the block, and both stacks. They differ in what happens next. ASan halts (the course sets
`halt_on_error=1`). Memcheck continues and prints `sum 6`: the invalid read returned 0 here, and
the program kept going. With memcheck, the *first* error is the one
to fix. Later ones may be consequences.

## Worked example: the wrong fix

`lesson-demo`'s `read_past` loops `i <= 4` over a 4-element block. A tempting "fix" is
`malloc(5 * sizeof *v)`: the report goes away and the sum is still 6, because the 5th
element is uninitialised. With that change, memcheck would report a conditional jump in
`printf` on an uninitialised value, and ASan would report nothing, since the read is now
in bounds. The **plausible wrong reading** is "the block was too small". The block matched
its contract of 4 elements. The loop violated it. The rule: decide which side of the
mismatch is the contract (here the allocation, sized for the data), and fix the other side.

## A method for unknown bugs

1. Run the cheapest discriminating tool first: `make asan`. Read the first report only.
2. Classify by the first line (`heap-buffer-overflow`, `heap-use-after-free`, ...).
3. Find the *cause* stack: for UAF the freed-by stack, for overflow the allocated-by stack.
4. Fix one bug, rebuild, rerun. Mixed-bag files contain several. Fixing one may
   reveal one that was masked, since ASan stops at the first.
5. When ASan is clean, run `make valgrind`. Uninitialised reads appear only there.

## `man` pointers

- `man 3 malloc`: NOTES (alignment, `MALLOC_CHECK_`), and `man 3 mallopt` for `M_PERTURB`.
- `man 7 signal`: why exit status 134 means `SIGABRT`.
- glibc's `malloc/malloc.c` source comments describe chunks, bins and tcache; the course
  does not require reading it.

## Check yourself

1. A program aborts with `malloc(): corrupted top size` inside `printf`. Where do you look?
2. ASan reports `heap-buffer-overflow` on a write at offset 48 of a 48-byte block of
   `struct {double x, y;}`. The code writes element 3. What is the likely class?
3. Why can `free(): double free detected in tcache 2` not catch every double free?

<details><summary>Answers</summary>

1. At the most recently allocated block before that `printf`: something wrote past its end
   into the top chunk's size field. Run ASan to see the actual overflowing write.
2. Under-allocation: 48 bytes holds 3 elements of 16, but the code expected more. Look for
   `sizeof p` (8) where `sizeof *p` (16) was meant, e.g. `malloc(6 * sizeof p)`.
3. It checks only chunks that are in the tcache at the second `free`. Chunks too large for
   tcache, or moved to other bins, escape it. Worse: if a `malloc` of the same size takes the
   chunk back out between the two frees, the second `free` releases the *new owner's* block,
   and that is a legal-looking free.
</details>

## Takeaways

- Classify by mechanism, fix the cause, change nothing else.
- Plain runs hide overflows inside chunk slack. A glibc abort points somewhere *after* the corruption.
- ASan first, memcheck second. Only memcheck sees uninitialised reads.

## Exercises

| Exercise | Tag | Size | Class |
|---|---|---|---|
| e01-leak | `[BUG]` | S | leak |
| e02-double-free | `[BUG]` | S | double free |
| e03-uaf | `[BUG]` | S | use after free |
| e04-heap-overflow | `[BUG]` | S | heap overflow |
| e05-heap-underflow | `[BUG]` | S | heap underflow |
| e06-off-by-one-nul | `[BUG]` | S | off-by-one NUL |
| e07-uninit-read | `[BUG]` | S | uninitialised read (Valgrind-graded) |
| e08-invalid-free | `[BUG]` | S | invalid free |
| e09-under-allocation | `[BUG]` | S | `sizeof p` vs `sizeof *p` |
| e10-glibc-aborts | `[BUG]` `[NO-ASAN]` | S | tcache double free, corrupted top size |
| e11-mixed-a | `[BUG]` | M | 2 bugs, classes not stated |
| e12-mixed-b | `[BUG]` | M | 3 bugs, classes not stated |
| e13-which-tool | `[PREDICT]` | S | which tool sees which bug |
| e14-strv-dup | `[FAULT]` | S | deep copy under allocation failure |
