# M02b: Your first ASan and Valgrind report

## Advance organizer

From M02 onward, `make check` runs your code under two independent tools, and sooner or
later one of them stops your program with a wall of text. This short unit teaches you to
read that text **top to bottom, in a fixed order**, so a report becomes a pointer to a line
of code rather than something to scroll past. It covers three report types:

1. an AddressSanitizer **heap-use-after-free** report;
2. a LeakSanitizer leak summary (LSan is part of ASan, and it runs at exit);
3. a Valgrind memcheck leak summary.

Deeper tool work (shadow memory, redzones, the four Valgrind leak categories in full,
`--track-origins`, and `ASAN_OPTIONS`) is M11. Here, you learn just enough to act on a report.

This lesson deliberately shows **no real tool output**. Every exercise ships a report
captured on this machine from its exact source, so you read the real thing, with your
machine's quirks, instead of a tidied textbook copy. Appendix C of
`LearnCwithRPG-terminal` shows a few more if you want to see some early.

## Foundations

### Two tools, two mechanisms, never on the same binary

- **ASan** is compiled in. The course's ASan build is really ASan plus UBSan
  (`-fsanitize=address,undefined -fno-sanitize-recover=all`), so a report can also be a
  UBSan `runtime error:` line, such as a null pointer passed to `memcpy`, or signed overflow.
  `-fsanitize=address` makes the compiler add a check before every
  load and store, and it replaces `malloc`/`free` with versions that surround each block
  with poisoned **redzones** and hold freed blocks in **quarantine** instead of reusing them
  right away. A bad access is caught *at the instruction that does it*. The program then
  aborts (the course sets `halt_on_error=1`).
- **Valgrind memcheck** runs an unmodified binary on a synthetic CPU and tracks every byte
  of memory. It is about 20-50x slower, needs no recompilation, and catches
  **uninitialised reads**, which ASan cannot. By default it reports and keeps going.

An ASan binary has its own allocator and a huge shadow mapping, and Valgrind can't run it
meaningfully. So the course builds `build/plain/` for Valgrind and `build/asan/` for ASan.
`make valgrind` and `make asan` choose the right one for you.

### Valgrind on Arch: make it start

On Arch, a bare `valgrind ./prog` can stop immediately with a "fatal error at startup"
about a mandatory function redirection that can't be set up. The reason: Arch ships a
stripped dynamic loader, and the debug packages the message recommends don't exist on Arch.
The fix is debuginfod, which lets Valgrind download the loader's symbols:

```bash
echo "$DEBUGINFOD_URLS"            # should print https://debuginfod.archlinux.org
export DEBUGINFOD_URLS=https://debuginfod.archlinux.org   # if it's empty
```

The course's `make valgrind` sets it for you. If you run Valgrind by hand, set it yourself.
`LearnCwithRPG-terminal/appendices/c-gdb-and-sanitizers.md` ("On Arch: it refuses to
start") and Appendix A cover the background.

### Reading an ASan report: the fixed order

Every ASan memory-error report has the same skeleton. Read it in this order:

1. **The headline**: the line starting `ERROR: AddressSanitizer:` names the **bug class**
   (`heap-use-after-free`, `heap-buffer-overflow`, `attempting double-free`, ...). It also prints the
   bad address and the registers, which you can usually ignore.
2. **The access line**: `READ of size N` or `WRITE of size N`, followed by a stack
   trace. Frame `#0` is the faulting instruction. Look down the frames for the **first one
   in your code**: that's file and line. `N` tells you the type that was accessed (4 for an
   `int`, 8 for a pointer or `double`, 1 for a `char`).
3. **The location line**: says where the address is *relative to a block*: "N bytes
   inside of", "N bytes after", or "N bytes before" a region of M bytes (older ASan versions
   say "to the right of" and "to the left of"). This line tells
   you which block, and which element of it.
4. **freed by thread T0 here**: (use-after-free and double-free only) the stack of the
   `free` that ended the block's lifetime.
5. **previously allocated by thread T0 here**: the stack of the `malloc` that created it.
6. **The shadow byte dump** and legend: skip it until M11.
7. **SUMMARY**: one-line repeat of the class and the top frame in your code.

For a use-after-free, you need three facts: where it was *used*, where it was *freed*, and
where it was *allocated*. Steps 2, 4, and 5 give you exactly those three. The bug is in the
relationship between them. Usually the free is too early, or the use goes through a stale
copy, as in M02 e03.

### Reading a LeakSanitizer report

LSan runs when the process exits. It scans the stacks, registers, and globals for anything
that looks like a pointer into a live heap block, follows those blocks transitively, and
reports every block it never reached. Read it in this order:

1. The headline: `ERROR: LeakSanitizer: detected memory leaks`.
2. One entry per **allocation site**: `Direct leak of N byte(s) in K object(s) allocated
   from:` plus a stack. **Direct** means nothing pointed at the block. **Indirect** means
   the block was pointed at only from other leaked blocks.
3. The SUMMARY line: total bytes and allocations.

Fix the **direct** leaks first. A leaked struct that owned a buffer shows up as a direct leak (the
struct) plus an indirect one (the buffer). Freeing the struct properly through its destroy
function usually makes both disappear.

### Reading a Valgrind memcheck summary

Valgrind prints each error as it happens, then a **HEAP SUMMARY** (blocks still in use at
exit, and total allocs and frees) and, with `--leak-check=full`, a **LEAK SUMMARY** split
into categories. For now, two of them matter:

- **definitely lost**: no pointer to the block exists anywhere, so it's a true leak. The course's
  Valgrind run fails on these.
- **indirectly lost**: only reachable from definitely-lost blocks. Also fails.
- (**possibly lost**, where only a pointer into the middle of a block survives, also fails the
  course run. **still reachable** doesn't. M11 explains both.)

Every line is prefixed `==PID==`. Each loss record shows the `malloc` stack, the same idea as LSan.
The final `ERROR SUMMARY` counts errors. The course runs with `--error-exitcode=1`, so a
non-zero count means a failed run.

## Worked example: reading before running

Given a use-after-free report whose access stack bottoms out at `board.c:41`, whose free
stack points at `reset_scores` in `scores.c:18`, and whose allocation stack points at
`scores_new` in `scores.c:7`, with "8 bytes inside of 40-byte region":

- 40 bytes, so, assuming `int` elements, a 10-element array from `scores_new`.
- 8 bytes inside, so element index 2 (byte 8 divided by `sizeof(int)`, which is 4).
- A read of size 4 at `board.c:41`, so the board read `scores[2]`, or an alias of it.
- Freed in `reset_scores`, so the board was holding the address from *before* the reset.

**Rejected reading:** "The bug is on `board.c:41`, so fix that line." Line 41 is where the
symptom showed up. The defect is that the board kept a pointer whose target `reset_scores`
was allowed to free. The fix belongs in the ownership relationship: update the board, or
have the board borrow through the owner on each use. ASan tells you *where* and *which
block*. Working out *why* is still your job.

## man pointers

- `man valgrind`: read the `--leak-check`, `--show-leak-kinds`, and `--error-exitcode` entries.
- `man gcc`: search for `-fsanitize=address`. The full ASan flag list lives in the
  sanitizer project's documentation, not in a man page. M11 points you there.

## Check yourself

1. Which three stacks does a heap-use-after-free report give you, and what question does each answer?
2. A report says "0 bytes after 40-byte region". What kind of bug is it, and
   which index was touched if the elements are `int`?
3. Why is a leaked buffer that belongs to a leaked struct reported as *indirect*?
4. Why can't you run Valgrind on `build/asan/prog`?
5. Which tool would you use for an uninitialised read, and why?

<details><summary>Answers</summary>

1. The access stack (where the bad read or write happened), the free stack (who ended the
   block's lifetime), and the allocation stack (who created the block).
2. A heap buffer overflow just past the end. With 4-byte `int`s the block holds indices
   0-9, so the access is to index 10: the classic `<=` instead of `<`.
3. It is still pointed to, but only from a block that is itself lost. Fixing the direct
   leak (freeing the owner properly) usually fixes it too.
4. Both replace `malloc`/`free`, and ASan reserves a huge shadow-memory region at fixed
   addresses that Valgrind's own layout conflicts with. The two can't share one process.
   Run Valgrind on `build/plain/prog`.
5. Valgrind (or MSan, which the course doesn't use). ASan doesn't track whether bytes were
   initialised; Valgrind does, and reports when an uninitialised value decides a branch.

</details>

## Takeaways

- Read ASan reports in order: class, access stack, location relative to block, free stack,
  alloc stack, summary.
- The symptom line isn't the bug. Use the three stacks to reconstruct the lifetime.
- LSan and Valgrind both point you at the **allocation** site of a leak. You then find the
  missing free.
- Fix direct leaks first. Indirect ones often follow.
- ASan and Valgrind use separate builds. On Arch, Valgrind needs `DEBUGINFOD_URLS`.

## Exercises

| # | Folder | Tag | Size | Topic |
|---|---|---|---|---|
| e01 | `e01-report-uaf/` | `[REPORT]` | S | Pinpoint a use-after-free from a shipped ASan report alone |
| e02 | `e02-report-lsan/` | `[REPORT]` | S | Map a LeakSanitizer report with direct and indirect leaks back to missing frees |
| e03 | `e03-predict-valgrind/` | `[PREDICT]` | S | Predict Valgrind's definitely and indirectly lost bytes and blocks, then run it |

M02b's exercises are `[REPORT]` and `[PREDICT]` only. Each is a complete program. Done
means your written answer matches, and you then fix the program until `make check` is clean.
