# M11: Reading the tools, in depth

## Organizer

M02b taught you to find the error line in a report. This unit teaches what each tool
*measures*, so that you can tell what a report proves, what a silent run does not prove,
and which tool a given bug class needs. You should know heap basics (M02-M04), ownership
(M05) and the build modes in `AUTHORING-INFRA.md`: `build/plain` for Valgrind, `build/asan`
for ASan+UBSan.

The unit is mostly `[REPORT]` and `[PREDICT]`: you read real output before you run anything.
Every excerpt below comes from `lesson-demo/` in this unit, captured on the course machine
with `make capture` (see `lesson-demo/Makefile`). Addresses and PIDs differ on your run.

## Foundations

### Two architectures, two blind-spot profiles

**AddressSanitizer** is compiler instrumentation plus a runtime. GCC inserts a check before
every load and store, and the runtime replaces `malloc`/`free`. Each heap block gets
*redzones* on both sides, and freed blocks sit in a *quarantine* (256 MB by default) before
reuse, so a late access to freed memory still hits poisoned bytes. Because the checks are
compiled in, ASan sees only code that was compiled with `-fsanitize=address`, and it costs
roughly 2x time and 2-3x memory.

**Valgrind memcheck** needs no recompilation. It runs your unmodified binary on a synthetic
CPU, translating machine code as it goes, and tracks two kinds of metadata. *A-bits* say
whether each byte is addressable. *V-bits* say whether each **bit** holds a defined value.
It replaces `malloc` with its own allocator, which places a redzone around each block. It
runs 20-50x slower.

The two cannot share a binary. An ASan executable maps a shadow region of terabytes of
virtual address space at fixed addresses at start-up, and installs its own `malloc`.
Valgrind's synthetic address space and its own `malloc` replacement collide with both.
That is why `make valgrind` runs `build/plain/prog` and never `build/asan/prog`.

### What each tool sees

| Bug | ASan | UBSan | Valgrind | Notes |
|---|---|---|---|---|
| heap overflow/underflow into a redzone | yes | - | yes | both miss a jump far enough to land in another live block |
| use after free (heap) | yes (quarantine) | - | yes | ASan misses it once quarantine has recycled the block |
| double free, invalid free | yes | - | yes | glibc alone often aborts too (M12) |
| stack buffer overflow | yes | bounds, if the array size is known | **no** | Valgrind has no redzones between stack objects |
| global buffer overflow | yes | bounds, if known | **no** | same reason |
| overflow inside one struct (array member into next member) | **no** | sometimes (bounds) | **no** | M13 e02 |
| use of stack memory after return | yes, while `detect_stack_use_after_return` is on | - | rarely | see below |
| leak | yes (LeakSanitizer) | - | yes, 4 categories | |
| **uninitialised read** | **no** | - | **yes** | ASan only tracks addressability |
| misaligned access | - | yes | no | M14 |

The uninitialised-read gap is the one people get wrong. ASan's shadow answers "may this
byte be touched?", never "has it been written?". It also *fills* fresh heap blocks with
`0xbe` (`malloc_fill_byte`, up to `max_malloc_fill_size` = 4096 bytes). An uninitialised
read therefore often gives a *different wrong value* under ASan than in the plain build. A
check may fail under `make asan` with no ASan report at all. That failure is a side effect
of the fill, not a detection. Exercises with uninitialised reads are graded by Valgrind.

### Shadow bytes

ASan maps every 8 application bytes (one *granule*) to one shadow byte, at
`(addr >> 3) + 0x7fff8000` on x86-64. Shadow `00` means all 8 bytes are addressable. A value
`k` from 1 to 7 means only the first `k` are. Negative values (printed as `fa`, `fd`, `f5`...)
mean none are, and say why. From the demo, `malloc(10)` followed by a write to `buf[10]`:

```
0x6c99b92e001a is located 0 bytes after 10-byte region [0x6c99b92e0010,0x6c99b92e001a)
...
=>0x6c99b92e0000: fa fa 00[02]fa fa fa fa fa fa fa fa fa fa fa fa
```

Each shadow byte on that line covers 8 bytes, so the row covers `0x...0000`-`0x...007f`.
`fa fa` is the 16-byte left redzone. `00` is block bytes 0-7. `[02]` is the granule holding
bytes 8-15, of which only 8 and 9 belong to the block. The brackets mark the granule of the
faulting address, and `0x...001a` is byte 10. One shadow byte tells you both that the access
lies in the block's last granule and how many bytes of that granule are real.

### The three stacks

A heap report can carry three stack traces, and you need all three:

- **accessed at**: the `READ`/`WRITE of size N` stack. This is *where it was noticed*, not
  necessarily the bug.
- **freed by**: for use-after-free, the last `free` (or moving `realloc`) of the block.
- **allocated by** (`previously allocated by` when freed): where the block was born.

The bug is usually a mismatch between two of them: an owner that freed early, or a pointer
kept across a call that may move the block. A `realloc` that moves a block shows up as
both the "freed by" and the "allocated by" frame. Exercise e02 turns on exactly that.

### Stack use after return

A returned pointer to a local array is not a heap bug. On the real stack, the dead frame is
ordinary addressable memory that the next call will reuse, so plain redzones cannot see the
access. GCC compiles every function so that, while the runtime flag
`detect_stack_use_after_return` is on, its locals live in a "fake frame" taken from a
separate allocator. On return the fake frame is poisoned with `f5` ("Stack after return") and
not reused immediately, so a later access is reported. The flag's default has changed over
time: older runtimes shipped it off. On this machine's GCC 16 libasan,
`ASAN_OPTIONS=help=1 ./prog` lists it as `(Current Value: true)` with nothing set, and
`detect_stack_use_after_return=0` makes the report disappear. Exercises set it explicitly
with `ASAN_EXTRA = detect_stack_use_after_return=1` so they do not depend on the default. Compare `-Wreturn-local-addr`, which catches only the literal `return &local;`
in the same function (M01), not a pointer laundered through another function.

### LeakSanitizer: direct and indirect

LSan runs at exit. It stops the program and scans the *roots*: globals, every thread's
stack, registers and TLS. It marks every block reachable from a root, and any unmarked
block is leaked. A leaked block is **direct** if nothing points to it at all, and
**indirect** if only other leaked blocks point to it. LSan never reports blocks still
reachable from a root. Demo: a global `config` block, plus a lost `Tag` that owns a label:

```
Direct leak of 8 byte(s) in 1 object(s) allocated from:
    #1 0x564841b83363 in leaks .../lesson-demo/main.c:26
Indirect leak of 48 byte(s) in 1 object(s) allocated from:
    #1 0x564841b83393 in leaks .../lesson-demo/main.c:29
SUMMARY: AddressSanitizer: 56 byte(s) leaked in 2 allocation(s).
```

The 32-byte `config` block is not mentioned, because a global still points to it. Fix the
direct leaks first: freeing the owner correctly usually makes the indirect ones disappear.

### Valgrind's four categories

Memcheck classifies each block left at exit by the pointers it finds in roots and in other blocks:

| Category | Meaning |
|---|---|
| definitely lost | no pointer to the block anywhere |
| indirectly lost | pointed to only from lost blocks |
| possibly lost | only *interior* pointers (into the middle) reach it |
| still reachable | a pointer to its start is still in a root |

The same demo under `valgrind --leak-check=full --show-leak-kinds=all`:

```
==797870== 32 bytes in 1 blocks are still reachable in loss record 1 of 3
==797870== 56 (8 direct, 48 indirect) bytes in 1 blocks are definitely lost in loss record 3 of 3
==797870==    definitely lost: 8 bytes in 1 blocks
==797870==    indirectly lost: 48 bytes in 1 blocks
==797870==    still reachable: 32 bytes in 1 blocks
```

**Plausible wrong reading:** "still reachable is harmless, it is only freed by the OS at exit."
The OS does reclaim it, but a block that is still reachable at exit is also a block
nobody freed. In a long-running loop that same code path accumulates blocks. "Reachable" is
a fact about the final instant, not about ownership. The course's `make valgrind` fails on
definite, indirect and possible leaks. It tolerates still-reachable ones, but the clean-exit
rule (free what you own) still applies.

### Uninitialised values and `--track-origins`

Memcheck does **not** complain when you copy an undefined value. Copies just propagate
V-bits. It reports only when undefinedness can change behaviour: a conditional jump, an
address computation, or a value passed to a system call. The report therefore points at
the *use*, often far from the missing assignment. `--track-origins=yes` (on in `make valgrind`)
records where each undefined value was born:

```
==797907== Conditional jump or move depends on uninitialised value(s)
==797907==    at 0x4001274: sign_of (main.c:37)
==797907==    by 0x40012C1: uninit (main.c:46)
==797907==  Uninitialised value was created by a stack allocation
==797907==    at 0x4001291: uninit (main.c:41)
```

Read the origin block first. Line 41 is the opening of `uninit`, the frame that declared
`total`. For stack origins the line is the function's frame, not the declaration.

### `ASAN_OPTIONS` you will actually use

Set as a colon-separated list: `detect_leaks=1` (LSan, on by default on x86-64 Linux),
`halt_on_error=1` (stop at the first report, needed for recoverable checks),
`abort_on_error=1` (die by `SIGABRT` instead of `_exit(1)`, so gdb and core dumps catch it),
`detect_stack_use_after_return=1`, `quarantine_size_mb=N`, `malloc_fill_byte=N`,
`max_malloc_fill_size=N`. `ASAN_OPTIONS=help=1 ./prog` prints them all.

### mtrace: a leak finder with no extra tools

glibc can log every `malloc`/`free` between `mtrace()` and `muntrace()` to the file named by
`MALLOC_TRACE`. Since glibc 2.34 the hooks live in `libc_malloc_debug.so`, so you must preload it:

```
MALLOC_TRACE=build/mtrace.log LD_PRELOAD=/usr/lib/libc_malloc_debug.so.0 ./build/plain/prog leaks
mtrace ./build/plain/prog build/mtrace.log
```

```
Memory not freed:
-----------------
           Address     Size     Caller
0x00005c2701e47010      0x8  at .../lesson-demo/main.c:26
0x00005c2701e47510     0x20  at .../lesson-demo/main.c:25 (discriminator 1)
0x00005c2701e47540     0x30  at .../lesson-demo/main.c:29
```

The list includes the 32-byte (`0x20`) `config` block that LSan ignored. mtrace does no
reachability analysis: it lists every block allocated and not freed while tracing was on.
It cannot tell you direct from indirect, and it sees only calls made between `mtrace()` and
`muntrace()`. It costs almost nothing, though, and runs on a plain build.

## Worked example: one bug, three tools

The lesson demo's `uninit` mode (above) under each tool: the plain build prints `sign: 1`
and exits 0. ASan reports nothing, because the variable is on the stack, is addressable,
and has no fill. Only memcheck reports, with the origin. A green `make asan` is not evidence
about initialisation.

## `man` and docs

- `man 1 valgrind`: `--leak-check`, `--show-leak-kinds`, `--errors-for-leak-kinds`, `--track-origins`
- `/usr/share/doc/valgrind/html/mc-manual.html`, section 4.2.8 "Memory leak detection"
- `man 3 mtrace`, `man 1 mtrace`
- ASan flags: `ASAN_OPTIONS=help=1 ./build/asan/prog`. The shadow byte legend is printed in every report.

## Check yourself

1. A check fails under `make asan` with no report, but passes in the plain build. What is the likeliest cause?
2. Why can `make valgrind` not use `build/asan/prog`?
3. A block is pointed to only by a pointer to its 10th byte, stored in a global. What does memcheck call it?

<details><summary>Answers</summary>

1. An uninitialised heap read: ASan fills new blocks with `0xbe` and the plain heap happened to be zero. Confirm with Valgrind.
2. ASan reserves a fixed, enormous shadow mapping and replaces `malloc`. Valgrind's address-space layout and allocator replacement conflict with both.
3. Possibly lost: only an interior pointer reaches it.
</details>

## Takeaways

- Read the report for what it proves; read the table above for what silence does not prove.
- Uninitialised reads: Valgrind with `--track-origins=yes`. Stack and global overflows: ASan.
- Direct leaks first. "Still reachable" is a statement about one instant, not about ownership.

## Exercises

| Exercise | Tag | Size | Tool |
|---|---|---|---|
| e01-shadow-bytes | `[REPORT]` | S | ASan shadow |
| e02-three-stacks | `[REPORT]` | S | ASan, realloc |
| e03-return-local | `[REPORT]` | S | ASan, stack after return |
| e04-direct-indirect | `[PREDICT]` | S | LSan |
| e05-four-categories | `[PREDICT]` | S | memcheck, all kinds |
| e06-track-origins | `[REPORT]` | S | memcheck |
| e07-split-fields | `[FAULT]` | M | sweep + mtrace |
