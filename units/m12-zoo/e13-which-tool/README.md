# m12-e13-which-tool `[PREDICT]`

## Goal

`main.c` has three scenarios, each with one deliberate defect: `nul`, `uninit` and `stack`.
For each scenario, predict what three builds will do:

| | plain (`build/plain/prog <name>`) | ASan+UBSan build | Valgrind on the plain build |
|---|---|---|---|
| `nul` | ? | ? | ? |
| `uninit` | ? | ? | ? |
| `stack` | ? | ? | ? |

For each cell, give the exit status (0, nonzero, abort) and, if a tool reports, the first
line of its report: the error kind and which tool (ASan, UBSan or Memcheck) produced it.

Write your table and one sentence of reasoning per cell to `PREDICTION.md` **before you run
anything**. Then run `make matrix`, which runs all nine cells and keeps only the report
headers. Compare, and under your table write what you got wrong and why.

## Constraints

- Do not edit `main.c`. The defects are the point.
- Predict from the source and LESSON.md, not from a run.

## Verify

There is nothing to `make check`. The answer is the reasoning. Compare with
`solutions/m12-zoo/e13-which-tool/NOTES.md` only after `PREDICTION.md` is written.

<details><summary>Hint</summary>

For the `stack` cell, look at `SANFLAGS` in `common.mk`, and ask which check
runs *before* the store.
</details>
