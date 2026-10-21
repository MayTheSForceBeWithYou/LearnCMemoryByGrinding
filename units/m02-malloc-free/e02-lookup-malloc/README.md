# M02 e02: Read the manual `[LOOKUP]`

## Goal

Answer the questions in `ANSWER.md` using **only** `man 3 malloc` (the page also covers
`free`, `calloc`, `realloc`, and `reallocarray`). Quote the sentence you relied on for each
answer. Then use `main.c`, a complete probe program, to check the answers that can be
observed on this machine.

Questions (also in the `ANSWER.md` template):

1. What can `malloc(0)` return? Is each possibility safe to pass to `free`?
2. Under what condition does `malloc` set `errno`, and to what?
3. What does the page say about Linux's overcommit, and what does that mean for a
   non-NULL result?
4. What does `free` do with a NULL pointer? With a pointer that was already freed?
5. Which environment variables in NOTES change allocator behaviour for debugging, and
   what does one of them do to freed or new memory?
6. Which of these functions are in C17, which are POSIX, and which are GNU or BSD
   extensions? (See the STANDARDS and HISTORY sections, or CONFORMING TO on older pages.)

## Constraints

- Quote, don't paraphrase from memory. If the page doesn't answer a question, say so.
- The probe is `main.c`. Run it three ways and record what changes:
  `make run ARGS=peek`, then `MALLOC_PERTURB_=165 make run ARGS=peek`, then
  `GLIBC_TUNABLES=glibc.malloc.tcache_count=0 MALLOC_PERTURB_=165 make run ARGS=peek`.
  If the second run looks like the first, explain why using what the third run shows.
- `peek` is for the plain build only. The comment at the top of `main.c` says why.

## How to verify

- `make run` builds and runs the probe. `make check` passes as shipped.
- Your answers are compared with the solution's NOTES.md.
