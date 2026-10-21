# Progress

Your ledger. Tick an exercise only when it is clean everywhere: checks, ASan/UBSan,
Valgrind, and the fault sweep where tagged. Use the Notes column for the thing you
got wrong first; that is the part worth rereading.

Legend: `[ ]` not started · `[~]` in progress · `[x]` clean everywhere

## M01: Lifetimes: stack, static, heap

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-trace-frames | `[TRACE]` | |
| [ ] | e02-return-local | `[BUG]` | |
| [ ] | e03-predict-warning | `[PREDICT]` | |
| [ ] | e04-three-ways | `[WRITE]` | |
| [ ] | e05-static-overwrite | `[PREDICT]` | |

## M02: `malloc` and `free` fundamentals

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-heap-squares | `[WRITE]` | |
| [ ] | e02-lookup-malloc | `[PREDICT]` | |
| [ ] | e03-use-after-free | `[BUG]` | |
| [ ] | e04-predict-free | `[PREDICT]` `[NO-ASAN]` | |
| [ ] | e05-dup-array | `[WRITE]` | |
| [ ] | e06-trace-free | `[TRACE]` | |

## M02b: Your first ASan and Valgrind report

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-report-uaf | `[REPORT]` | |
| [ ] | e02-report-lsan | `[REPORT]` | |
| [ ] | e03-predict-valgrind | `[PREDICT]` | |

## M03: `calloc`, zeroing, size arithmetic

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-zeroed-tally | `[WRITE]` | |
| [ ] | e02-break-count | `[PREDICT]` | |
| [ ] | e03-checked-array | `[WRITE]` | |
| [ ] | e04-predict-conversions | `[PREDICT]` | |
| [ ] | e05-negative-count | `[BUG]` | |

## M04: `realloc` done right

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-append-grow | `[WRITE]` | |
| [ ] | e02-lost-on-failure | `[BUG]` `[FAULT]` | |
| [ ] | e03-trace-move | `[TRACE]` | |
| [ ] | e04-predict-realloc | `[PREDICT]` `[NO-ASAN]` | |
| [ ] | e05-dangling-interior | `[BUG]` | |
| [ ] | e06-fault-growbuf | `[FAULT]` | |

## M05: Ownership and create/destroy pairs

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-who-frees-this | `[TRACE]` | |
| [ ] | e02-design-contract | `[DESIGN]` | |
| [ ] | e03-out-param | `[WRITE]` | |
| [ ] | e04-borrowed-freed | `[BUG]` | |
| [ ] | e05-design-opaque | `[DESIGN]` | |
| [ ] | e06-transfer | `[WRITE]` | |

## M06: Partial failure and cleanup

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-two-step | `[FAULT]` | |
| [ ] | e02-leak-on-third | `[BUG]` `[FAULT]` | |
| [ ] | e03-goto-cleanup | `[FAULT]` | |
| [ ] | e04-predict-sweep | `[PREDICT]` `[FAULT]` | |
| [ ] | e05-idempotent-destroy | `[FAULT]` | |
| [ ] | e06-nested-build | `[WRITE]` `[FAULT]` | |

## M07: Heap strings

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-my-strdup | `[WRITE]` | |
| [ ] | e02-off-by-one-nul | `[BUG]` | |
| [ ] | e03-join | `[WRITE]` | |
| [ ] | e04-fault-format | `[FAULT]` | |
| [ ] | e05-predict-lengths | `[PREDICT]` | |
| [ ] | e06-design-paths | `[DESIGN]` | |

## M08: Arrays of pointers and 2D grids

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-split-words | `[WRITE]` | |
| [ ] | e02-free-order | `[BUG]` | |
| [ ] | e03-lost-table | `[PREDICT]` | |
| [ ] | e04-flat-grid | `[WRITE]` | |
| [ ] | e05-one-block | `[WRITE]` | |
| [ ] | e06-copy-rows | `[FAULT]` | |

## M09: The dynamic array

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-vec-int | `[WRITE]` | |
| [ ] | e02-insert-remove | `[WRITE]` | |
| [ ] | e03-growth-count | `[PREDICT]` | |
| [ ] | e04-held-pointer | `[REPORT]` | |
| [ ] | e05-strvec | `[DESIGN]` `[FAULT]` | |
| [ ] | e06-vec-under-fire | `[FAULT]` | |

## M10: Linked structures

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-list-basics | `[WRITE]` | |
| [ ] | e02-free-loop | `[BUG]` | |
| [ ] | e03-who-is-lost | `[PREDICT]` | |
| [ ] | e04-tree-free | `[WRITE]` | |
| [ ] | e05-shared-tail | `[REPORT]` | |
| [ ] | e06-list-from-words | `[FAULT]` | |

## M11: Reading the tools, in depth

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-shadow-bytes | `[REPORT]` | |
| [ ] | e02-three-stacks | `[REPORT]` | |
| [ ] | e03-return-local | `[REPORT]` | |
| [ ] | e04-direct-indirect | `[PREDICT]` | |
| [ ] | e05-four-categories | `[PREDICT]` | |
| [ ] | e06-track-origins | `[REPORT]` | |
| [ ] | e07-split-fields | `[FAULT]` | |

## M12: Bug hunt: the heap bug zoo

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-leak | `[BUG]` | |
| [ ] | e02-double-free | `[BUG]` | |
| [ ] | e03-uaf | `[BUG]` | |
| [ ] | e04-heap-overflow | `[BUG]` | |
| [ ] | e05-heap-underflow | `[BUG]` | |
| [ ] | e06-off-by-one-nul | `[BUG]` | |
| [ ] | e07-uninit-read | `[BUG]` | |
| [ ] | e08-invalid-free | `[BUG]` | |
| [ ] | e09-under-allocation | `[BUG]` | |
| [ ] | e10-glibc-aborts | `[BUG]` `[NO-ASAN]` | |
| [ ] | e11-mixed-a | `[BUG]` | |
| [ ] | e12-mixed-b | `[BUG]` | |
| [ ] | e13-which-tool | `[PREDICT]` | |
| [ ] | e14-strv-dup | `[FAULT]` | |

## M13: gdb for memory bugs

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-break-on-alloc | `[TRACE]` | |
| [ ] | e02-far-corruption | `[BUG]` | |
| [ ] | e03-asan-stop | `[FAULT]` | |
| [ ] | e04-vgdb | `[BUG]` | |

## M14: Arenas and pools (intuition)

| | Exercise | Tag | Notes |
|---|---|---|---|
| [ ] | e01-bump-arena | `[WRITE]` | |
| [ ] | e02-misaligned | `[BUG]` | |
| [ ] | e03-pool-freelist | `[WRITE]` | |
| [ ] | e04-heap-usage | `[PREDICT]` | |
| [ ] | e05-arena-chain | `[FAULT]` | |

## Capstones

| | Capstone | Tag | Notes |
|---|---|---|---|
| [ ] | c1-wordfreq | `[FAULT]` | |
| [ ] | c2-gridload | `[FAULT]` | |
| [ ] | c3-lru | `[FAULT]` | |
