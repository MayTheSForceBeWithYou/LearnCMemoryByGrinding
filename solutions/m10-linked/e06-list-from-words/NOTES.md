# m10-e06-list-from-words: notes

A `Node **link` tail cursor gives O(1) append. A node joins the list only once its `name` copy exists, so
the single `fail:` path can hand the whole list to `list_destroy`. The one node that isn't linked yet (when
`strdup` failed) is freed on its own first.

**Sweep.** `sweep N=10 pass=0 oom=10 fail=0`: 5 words × (node + name). Every failure point exits 3 with
nothing leaked.

**n == 0 vs failure.** The harness can tell them apart only because it knows `n`: for `n > 0`, NULL can only
mean OOM. A better API returns a status and the list through an out-parameter
(`bool list_from_words(const char *const *, size_t, Node **out)`), so an empty list is `true` with `*out == NULL`.
