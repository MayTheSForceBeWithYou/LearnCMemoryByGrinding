# M06 e02 notes

- The sweep has N = 8 (4 per session). `FAIL_AT=3` (session a's buffer) and `FAIL_AT=7` (session b's buffer) leak the user copy:
  4 bytes (`"ada"`) and 7 bytes (`"brigid"`), allocated by `strdup` in `session_create`.
- The defect: `goto fail_user` after the buffer allocation fails. That label frees only the struct. The step that failed is the *buffer*,
  so the jump must go to `fail_buf`, which frees the user copy and falls through to the struct.

| fails at | acquired | must release |
|---|---|---|
| struct | nothing | nothing |
| user | struct | struct |
| buf | struct, user | user, struct |
| history | struct, user, buf | buf, user, struct |

- Rule of thumb for the ladder: **jump to the label named after the step that failed.** The label's code frees the steps *before* it.
  Here the label names were right, and the `goto` was copy-pasted from the line above.
