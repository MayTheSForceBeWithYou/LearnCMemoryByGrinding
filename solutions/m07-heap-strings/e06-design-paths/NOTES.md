# M07 e06 notes

- Three results are owned, and one is borrowed. The return type tells you which: `const char *` for the borrow. The comment carries the lifetime
  rule, because the type can't express it.
- `path_with_ext` searches for the dot only inside the basename, so `v1.2/readme` doesn't get cut to `v1`.
- `path_dirname` reuses `strdup`/`strndup`, which are exactly sized and return NULL on failure, so they need the POSIX macro.
- The last block in `main.c` is the point of the exercise: after `free(owned)`, `base` is dangling even though nothing ever freed `base`.
  Reading it then would be a heap-use-after-free on `owned`'s block.
- Known gaps, kept out of the spec on purpose: trailing slashes in `path_dirname("a/b/")` and dotfiles like `".bashrc"`. The second is
  treated as having extension `.bashrc`, which `basename(1)`-style tools don't do. Worth a sentence in a code review.
