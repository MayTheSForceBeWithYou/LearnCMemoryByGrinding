# M05 e03 notes

- Three allocations (struct, key, value), so four outcomes. The label ladder frees exactly what exists at each failure point.
  It's a preview of M06. `make fault` passes on this solution (N = 3 per successful parse).
- `*out = p` is the last statement before `return 0`. Writing `*out` early and then failing would hand the caller a pointer to a freed struct.
- `strndup(line, eq - line)` copies the key without modifying `line`. Writing a `'\0'` over the `=` would violate the borrow (and `line` is `const`).
- `"=="` is malformed because the key is empty. `"k=v=w"` splits at the first `=`, a choice the harness pins down.
- `p = SENTINEL` before each call is how the harness *proves* that `*out` is untouched on failure. NULL would be a weaker test.
