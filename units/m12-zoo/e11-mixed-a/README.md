# m12-e11-mixed-a `[BUG]`

## Goal

A phone book stores owned names and numbers in a growable array. **Two** bugs, of
unspecified classes. The plain build crashes.

Find and fix both. Write `ANSWER.md` with one section per bug: what was wrong, the
mechanism, and which tool report revealed it. Say which bug was masking the other and why.

## Constraints

- Exactly two defects. Each fix is local.
- `book_add` must still leave the book unchanged on allocation failure.

## Verify

`make check` passes.
