# M06 e01 notes

- Failure table: step 1 (struct) fails, so nothing to undo. Step 2 (pixels) fails, so free the struct. That's the whole ladder, and at two
  steps an `if` is clearer than labels.
- `w > IMAGE_MAX_PIXELS / h` decides `w * h > MAX` without computing the product. `w * h` in `int` overflows for
  `INT_MAX x INT_MAX`, which is UB, and UBSan aborts on it. `(size_t)w * (size_t)h` is safe after the check.
- `calloc` gives zeroed pixels and its own overflow check. We still need ours, because the limit is a policy, not a size_t bound.
- Sweep: three images make 6 allocation calls. Every `FAIL_AT` makes one create return NULL, and `main` destroys the earlier ones and
  exits 3. Expected summary: `N=6 pass=0 oom=6`.
