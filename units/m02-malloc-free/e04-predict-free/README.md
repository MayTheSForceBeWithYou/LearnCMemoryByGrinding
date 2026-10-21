# M02 e04: What does this `free` do? `[PREDICT]` `[NO-ASAN]`

## Goal

`main.c` contains six scenarios, `s1` to `s6`, selected by a command-line argument
(`./main 3` runs scenario 3). They include `free(NULL)`, a correct free followed by
`p = NULL`, a double free with nothing in between, a double free with another `malloc`
in between, freeing a stack array, and freeing `p + 1`.

For each scenario, predict in `ANSWER.md` one of:

- **defined, no output:** the program exits 0;
- **glibc aborts:** write the message you expect it to print (from glibc, not from a
  sanitizer) and the exit status;
- **undefined but silent:** it runs and exits 0, yet the program is wrong.

Then run each scenario on the **plain** build and record what really happened.

## Constraints

- This exercise is `[NO-ASAN]` on purpose: you're seeing what glibc alone does. Run
  `build/plain/prog N` (or `make run ARGS=N`). Don't use `make asan` until your
  predictions are recorded. Then run it once to see how ASan names each case.
- The build prints `-Wuse-after-free` warnings for some scenarios. Note which ones: that is a
  compile-time prediction made for you, and it is part of the data.
- Where an outcome surprised you, write one sentence on why glibc can or can't detect that
  case cheaply.

## How to verify

- `make check` runs only the defined scenarios (1 and 2) and passes as shipped.
- Your table is compared with the solution's NOTES.md, which is based on captures from
  the exact shipped source.
