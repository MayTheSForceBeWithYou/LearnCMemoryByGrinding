# m10-e04-tree-free: notes

`tree_add` walks a `TreeNode **` down to the NULL link where the key belongs, or to the node that already
has it (increment, no allocation). The node and its key copy are created together, and either failure frees
what exists, so the tree is untouched. `tree_count` is the same walk, read-only. `tree_free` is post-order:
children, then the key, then the node.

**Depth limit (for `tree.h`).** Recursion depth equals the tree height, which is n for sorted input. On the
course machine (`ulimit -s` 8192), a degenerate tree with this node shape freed fine at 250,000 nodes and crashed
at 300,000, in both the plain and the ASan build. The harness's 3,000 is far below that.

**Alternatives.** Iterative free with no stack: repeatedly rotate the left child up (`if (t->left) { rotate
right } else { free t, go right }`). O(n) and O(1) space.

**The usual mistake.** Freeing the node before recursing into its children (reads freed `left`/`right`), or
forgetting the key.
