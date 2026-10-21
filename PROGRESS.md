# Progress

Your ledger. Tick an exercise only when it is clean everywhere: checks, ASan/UBSan,
Valgrind, and the fault sweep where tagged. Use the Notes column for the thing you
got wrong first; that is the part worth rereading.

Legend: `[ ]` not started · `[~]` in progress · `[x]` clean everywhere

## Course build state (authors)

| Part | Owner | State |
|---|---|---|
| `common.mk`, `lib/`, `scripts/` | Bruce | in use |
| M01-M07, C1 | Monk | not started |
| M08-M10 lessons + exercise READMEs | Jack | revised from real captures + Monk's review |
| M08-M10 starters + solutions | Jack | done, verify-all 35/35 green (2026-10-21), awaiting Bruce |
| C2 README | Jack | revised (format edge cases, NULL rules) |
| C2 starter + solution | Jack | done, verify-all 2/2 green, sweep N=46, awaiting Bruce |
| M11-M14, C3 | Bruce | not started |

## M01-M07, M02b

Exercise rows are added by their author when the unit lands.

| Unit | Done | Notes |
|---|---|---|
| M01 Lifetimes | 0/5 | |
| M02 `malloc`/`free` | 0/6 | |
| M02b First reports | 0/3 | |
| M03 `calloc` and sizes | 0/5 | |
| M04 `realloc` | 0/6 | |
| M05 Ownership | 0/6 | |
| M06 Partial failure | 0/6 | |
| M07 Heap strings | 0/6 | |

## M08: Arrays of pointers and 2D grids

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-split-words | `[WRITE]` S | |
| [ ] | e02-free-order | `[BUG]` | |
| [ ] | e03-lost-table | `[PREDICT]` | |
| [ ] | e04-flat-grid | `[WRITE]` M | |
| [ ] | e05-one-block | `[WRITE]` M (`long double` cells) | |
| [ ] | e06-copy-rows | `[FAULT]` S | |

## M09: The dynamic array

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-vec-int | `[WRITE]` M | |
| [ ] | e02-insert-remove | `[WRITE]` S | |
| [ ] | e03-growth-count | `[PREDICT]` | |
| [ ] | e04-held-pointer | `[REPORT]` | |
| [ ] | e05-strvec | `[DESIGN]` `[FAULT]` | |
| [ ] | e06-vec-under-fire | `[FAULT]` | |

## M10: Linked structures

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-list-basics | `[WRITE]` M | |
| [ ] | e02-free-loop | `[BUG]` | |
| [ ] | e03-who-is-lost | `[PREDICT]` | |
| [ ] | e04-tree-free | `[WRITE]` M | |
| [ ] | e05-shared-tail | `[REPORT]` | |
| [ ] | e06-list-from-words | `[FAULT]` | |

## M11-M14

| Unit | Done | Notes |
|---|---|---|
| M11 Reading the tools | 0/7 | |
| M12 Bug zoo | 0/10 | |
| M13 gdb | 0/4 | |
| M14 Arenas and pools | 0/5 | |

## Capstones

| | Capstone | Notes |
|---|---|---|
| [ ] | C1 `wordfreq` | |
| [ ] | C2 `gridload` | |
| [ ] | C3 `lru` | |
