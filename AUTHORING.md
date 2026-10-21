# Authoring rules

Built from `PLAN.md` v0.2, sections 4 and 7. When this file and `PLAN.md` disagree,
`PLAN.md` wins. Fix this file and tell the other authors.

Build mechanics (the exercise Makefile template, `TAG`, `EXPECT_SYMBOLS`, `make capture`,
`capture.txt`, `verify-all.sh` usage) are in **`AUTHORING-INFRA.md`**. This file covers content rules.

## 1. Order of work

1. The shared infrastructure comes first: `common.mk`, `lib/` (`failalloc`, `course.h`) and `scripts/verify-all.sh`.
   **No exercise code is written before they exist.** Lessons and READMEs may be drafted earlier.
2. For each unit, write `LESSON.md` first, then the exercises, then the solutions.
3. Run `scripts/verify-all.sh`. A unit is not done until it passes, and the reviewer has signed it off.

## 2. Folder layout

```
units/mNN-<slug>/
├── LESSON.md
└── eNN-<slug>/
    ├── README.md
    ├── Makefile           # TAG, SRCS, EXPECT_SYMBOLS, include ../../../common.mk (see AUTHORING-INFRA.md)
    ├── main.c             # starter + self-checking harness
    └── (other .c/.h, data files)
solutions/mNN-<slug>/eNN-<slug>/
├── main.c (and friends)   # complete, compilable reference
└── NOTES.md               # why it is correct, alternatives, the usual mistake
capstones/cN-<name>/       # same shape; solutions/capstones/cN-<name>/ mirrors it
```

- Solutions are complete programs, never next to the exercise, and **no README links to them**.
- Exercise ids are `mNN-eNN-<slug>` (e.g. `m08-e01-split-words`) and capstone ids are `cN-<name>`.
  The harness ends with `return checks_done("<id>");`, which prints `<id>: all checks passed`.
  Always use the full-folder form (`m08-e01-split-words`), never the short `m08-e01`.

## 3. LESSON.md

120-250 lines. Contents:

- **Organizer:** what the unit is for, what the reader should already know, and where it fits in the course.
- **Foundations:** the idea and the teaching.
- **Worked example(s):** 1-2. At least one shows a *plausible wrong reading* (what a competent
  person would reasonably but wrongly conclude) and why it is wrong.
- **`man` pointers:** exact pages and sections.
- **Check yourself:** short questions, answers in `<details>`.
- **Takeaways.**
- **Exercise list with tags and sizes.**

Do not solve an exercise in the lesson. Worked examples use neighbouring problems.

## 4. Exercise READMEs

- State the **goal**, the **constraints** and **how to verify**. Never give a step list that
  dictates the implementation. Clear, not spoon-fed.
- At most **one** hint, in a collapsed `<details>` block.
- Constraints are concrete and testable ("must not leak if the 3rd allocation fails").
- Every `[WRITE]`/`[FAULT]`/`[DESIGN]` README carries the build-failure notice (section 5).
- `[BUG]` READMEs ask for `ANSWER.md`: what was wrong, the mechanism, and how the tool showed it.

## 5. Starters: what builds and what does not

| Tags | Starter state |
|---|---|
| `[WRITE]`, `[FAULT]`, `[DESIGN]` | **Fails to build until the student writes the code.** Prototypes only (in `main.c` or a provided header). The harness calls them. Link fails with `undefined reference`. **No dummy stubs, no `return 0;` placeholders, no stub stop lines.** |
| `[BUG]`, `[REPORT]`, `[PREDICT]`, `[TRACE]` | Complete programs that build. `[BUG]` builds, then fails its check, a sanitizer or Valgrind. |

For `[DESIGN]`, the header may ship without the contract comments. The student writes them.
`verify-all.sh` enforces both rows.

Standard notice for README files (copy verbatim):

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.

## 6. Harness, exit codes, OOM

- Use `check(cond, msg)` from `lib/course.h`. On success, print `<id>: all checks passed` and exit 0.
  On failure, exit 1.
- **Graceful allocation failure:** print one line to stderr and exit **`EXIT_OOM` (3)** from
  `lib/course.h`. Every `[FAULT]` exercise and every capstone does this, and must not leak on that path.
  "Graceful" means everything allocated so far is freed before exit, so LeakSanitizer stays silent.
- Other non-zero codes are only allowed when a README defines them (e.g. capstone 2's input errors).

## 7. Done means clean everywhere

A solution passes all of:

1. its checks;
2. the ASan + UBSan build (`-fsanitize=address,undefined -fno-sanitize-recover=all
   -fno-omit-frame-pointer` in **both** CFLAGS and LDFLAGS), run with
   `ASAN_OPTIONS=detect_leaks=1:abort_on_error=1:halt_on_error=1:allocator_may_return_null=1`
   (huge or failed allocations return NULL instead of aborting, so the overflow tests grade the code the student wrote);
3. the plain build under
   `valgrind --leak-check=full --errors-for-leak-kinds=definite,indirect,possible --error-exitcode=1`;
4. for `[FAULT]` and capstones: the sweep (`FA_COUNT=1` to get N, then `FAIL_AT=1..N` under ASan),
   where every run passes or exits 3, with zero leaks and no report.

Base flags: `-std=c17 -Wall -Wextra -Wpedantic -g`.
ASan and Valgrind never run on the same binary: `build/plain/`, `build/asan/`, `build/fault/`.

**Grade with the tool that can see the bug.** ASan does not detect uninitialised reads. Those
exercises are graded by Valgrind memcheck. MSan is out of scope.

`[NO-ASAN]` exercises deliberately run without sanitizers. Flag them in the tag line and the README,
and say what "done" means for them.

## 8. Fault injection (`failalloc`)

- Link-time `--wrap` for `malloc`, `calloc`, `realloc`, `reallocarray`, `free`, `strdup` and `strndup`.
  Students write plain `malloc`. There is no `fa_*` API.
- `FAIL_AT=N` fails the Nth allocation. A failed `realloc` returns NULL **and leaves the original block alive**.
- `FA_COUNT=1` prints the allocation count to stderr on exit.
- **Limitation:** `--wrap` only sees calls from the exercise's own object files. Allocations inside
  libc (`getline`, `asprintf`, `fopen`'s buffers) are invisible. Either wrap `getline` explicitly,
  or keep exercises that use it out of `[FAULT]`. Exercise and capstone READMEs that read files say
  which functions are off-limits for this reason.

## 9. Unit composition

Per unit: at most 3 `[WRITE]`, at least 1 `[BUG]` or `[REPORT]`, at least 1 `[PREDICT]` or `[TRACE]`.
From M06 onward, at least 1 `[FAULT]`.
Sizes: S (< 40 lines of student code), M (40-150), L (150-300, capstones only).

## 10. Bugs and tool output

- **Bug exercises contain exactly the advertised bugs** and nothing accidental. A second author confirms.
- **Never invent tool output.** Paste real ASan, Valgrind and gdb output captured on the target machine
  **from the exact shipped source**. Re-capture whenever that source changes. `verify-all.sh` does not
  re-run the tools or diff output: it recomputes the source hash and compares it with each capture's
  `# source-sha256:` header, flagging a mismatch as STALE.
- `[REPORT]` files note that addresses will differ on the student's run.
- Captures are made with `make capture TOOL=asan|valgrind|run`. It writes `capture.txt` with a
  `# source-sha256:` header, and `verify-all.sh` flags it STALE when the source changes. README
  excerpts are copied from `capture.txt`, never typed.
- `[PREDICT]` answer keys may keep their captures in the solution dir as `capture*.txt` (no Makefile there);
  `verify-all.sh` checks them against the starter's source hash. A lesson that quotes tool output needs a
  `units/<unit>/lesson-demo/` program with its own capture, or it must not quote the output.
- Until a capture exists, leave a marker where it goes, naming the exact command:

  ```
  <!-- OUTPUT-PENDING: cd units/m09-vec/e04-held-pointer && make capture TOOL=asan -->
  ```

  `grep -rn OUTPUT-PENDING units capstones` must be empty before a unit is marked done.

## 11. Voice

Adult, specific, plain. Say what happens and why. No cheerleading, no filler, no "simply".
Cross-reference `LearnCwithRPG-terminal` ch10/ch11 and its appendix C where it helps, but every
lesson stands on its own.

## 12. Before you mark a unit done

- [ ] LESSON.md within 120-250 lines, exercise list matches the folders
- [ ] composition rules (section 9) hold
- [ ] each starter builds or fails to build as its tag requires
- [ ] every solution passes section 7 (and section 8 sweep where tagged)
- [ ] no `OUTPUT-PENDING` markers remain; every pasted output came from the shipped source
- [ ] bug exercises confirmed by a second author
- [ ] `scripts/verify-all.sh` passes; `PROGRESS.md` and the README course map updated
