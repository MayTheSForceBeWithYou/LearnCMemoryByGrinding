# M06 e05 notes

- The invariant ("each owned field is NULL or live") is what lets one function handle seven partial states (struct only, + label, + samples,
  + slot 0..2). Writing it down is the deliverable. Without it, the next person to add a field will break destroy.
- `calloc` for the struct is essential. With `malloc`, a failure at `label` would leave `samples` and the slots holding garbage, and destroy
  would free garbage.
- "Idempotent" in the narrow sense: destroy only ever frees fields that are NULL or live. It does **not** make a second
  `widget_destroy(w)` on the same pointer safe, because the struct itself is gone.
- Reverse order in destroy isn't required for correctness here (the blocks are independent), but it mirrors construction and costs nothing.
- Sweep: 6 calls per widget, 12 total, and all of them make `main` exit 3.
