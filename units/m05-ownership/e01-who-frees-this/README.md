# M05 e01: Who frees this? `[TRACE]`

## Goal

`main.c` calls twelve functions from three small modules (`cfg.c`, `names.c`, `fmt.c`).
Some return owned heap blocks, some return borrows into their module's state, some return
static buffers, and one returns a string literal. The headers have **no ownership comments**.
You'll have to read the implementations.

For each of the twelve call sites marked `/* WHO-1 */` to `/* WHO-12 */`, fill in a row in `ANSWER.md`:

| site | owned / borrowed / static / literal | who must free (or "nobody") | valid until (name the call or event) |
|---|---|---|---|

Then trace the two marked points `/* STATE A */` and `/* STATE B */`: list every live heap
block, its owner, and every pointer variable in `main` that refers to it. Mark any that
dangle.

## Constraints

- Work from the source. Then run `make check`, which passes, since the shipped `main` handles
  every pointer correctly. Check your table against what `main` actually frees.
- Finally, write the missing ownership comment on each of the twelve declarations in the three
  headers, in the lesson's style. Those comments are part of the deliverable.

## How to verify

- `make check` passes before and after your header edits. You changed comments only.
