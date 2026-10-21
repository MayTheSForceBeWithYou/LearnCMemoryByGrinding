#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

/* A binary search tree of string keys with counts, ordered by strcmp.
 * The tree owns every node; each node owns its key. NULL is the empty tree.
 * Depth limit: tree_free recurses once per level, so its depth is the tree height. Keys added in
 * sorted order build a degenerate tree whose height is the node count. With an 8 MiB stack
 * (ulimit -s 8192) a degenerate tree freed fine at 250,000 nodes and overflowed the stack at
 * 300,000 on the course machine (units/m10-linked/lesson-demo). Keep inputs well below that. */
typedef struct TreeNode {
    char            *key;
    int              count;
    struct TreeNode *left, *right;
} TreeNode;

/* Inserts a copy of key with count 1, or increments its count if present (allocating nothing).
 * key is borrowed for the call. On failure returns false; the tree is unchanged, nothing leaked. */
bool tree_add(TreeNode **root, const char *key);
/* The count for key, or 0 if absent. */
int  tree_count(const TreeNode *root, const char *key);
/* Frees every key and node. Accepts NULL. */
void tree_free(TreeNode *root);

#endif
