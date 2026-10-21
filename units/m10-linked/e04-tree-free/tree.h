#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

/* A binary search tree of string keys with counts, ordered by strcmp.
 * The tree owns every node; each node owns its key. NULL is the empty tree.
 * TODO: state the recursion depth limit of tree_free and the input that reaches it. */
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
