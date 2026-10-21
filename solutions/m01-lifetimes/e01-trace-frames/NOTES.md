# M01 e01 notes

The program is the solution. The answer is the drawing.

- **Static column, at every point (A, B, C):** `calls` (in `inner`). It's 0 before the first call, so at
  point A it's already 1 (first call) or 2 (second). It's never in a frame.
- **Point A, first call:** frames `main` (`first` = ?, `second` = ?) -> `outer` (`n`=3,
  `total`=7, `heap` -> block H1 of 4 bytes holding 103, `r` = ?, `result` = ?) -> `inner` (`acc` ->
  `outer.total`, `local`=6). The heap holds H1 only.
- **Point B, first call:** the `inner` frame is gone (`local` died at its closing brace, and `acc` with it). `r` points
  at `total` (still alive, since it's in `outer`'s frame). H1 is still live, 103.
- **Point C:** only `main`'s frame is live: `first`=110, `second`=111. The heap is empty. H1 was freed in
  the first `outer` and a new block (H2) was created and freed in the second. The two `outer` frames
  are dead. `calls`=2.
- **Common mistake:** putting `calls` inside `inner`'s frame, or showing it as 0 at point A of the second call.
  Static locals are initialised once, before `main`, not on each entry.
- `r` returned by `inner` is valid because it points into the *caller's* frame, which outlives the call.
  Contrast this with e02.
