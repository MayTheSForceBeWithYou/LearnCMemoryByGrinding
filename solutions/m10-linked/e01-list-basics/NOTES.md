# m10-e01-list-basics: notes

`node_new` makes the node and the name copy as one unit, so a failure at either step frees what exists and
returns NULL before the list is touched. Both pushes therefore leave the list unchanged on failure.
`push_back` and `remove_first` walk a `Node **` from `head`, so the first node is just another link: no
head special case. `remove_first` unlinks (`*pp = n->next`) before it frees, and `destroy` reads `next`
before freeing each node.

**The usual mistakes.** Forgetting to free `name` (LeakSanitizer reports every name as a *direct* leak, one
record per allocation site). Advancing `pp` after an unlink in a remove-all loop (see the lesson). In
`push_back`, walking with a `Node *` and then needing a special case when the list is empty.
