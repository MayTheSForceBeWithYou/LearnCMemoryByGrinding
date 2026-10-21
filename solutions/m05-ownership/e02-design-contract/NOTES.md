# M05 e02 notes

- **The contract decides the implementation.** "name stable across add_tag" means the name lives in its own block, not in a shared
  arena that might be reallocated. "setters unchanged on failure" means copy first, then free the old value, then commit.
- `contact_create` uses `calloc`, so a half-built contact has NULL fields, and `contact_destroy` can be its cleanup. That's the M06 idiom, introduced early.
- `contact_add_tag` copies first, grows second, and frees the copy if the grow fails. The reverse order would need to undo the grow, which you can't.
- `contact_to_string` sizes exactly: `len(name) + 2 + len(email) + 3 + 1` (" <", "> [", "]"), plus the tags,
  `n - 1` commas, and the NUL. (The author forgot the "]" in a first draft, and ASan caught it at once: count delimiters one by one.) `sprintf` is safe here only because we computed the size. M07 swaps this for `snprintf(NULL, 0, ...)`.
- Common contract gaps: not saying whether `name` is copied or retained, not saying what invalidates `contact_name`'s result, and
  forgetting the failure state of setters.
