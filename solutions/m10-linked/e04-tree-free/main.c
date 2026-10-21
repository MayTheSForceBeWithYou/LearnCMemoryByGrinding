/* m10-e04-tree-free: a BST of owned keys; recursive free. */
#define _POSIX_C_SOURCE 200809L    /* strdup() under -std=c17; before any #include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"
#include "tree.h"

enum { SORTED = 3000 };    /* sorted keys: a degenerate, list-shaped subtree of this depth */

static int oom(TreeNode *root)
{
    tree_free(root);
    fprintf(stderr, "m10-e04-tree-free: out of memory\n");
    return EXIT_OOM;
}

int main(void)
{
    TreeNode *root = NULL;
    check(tree_count(root, "x") == 0, "empty tree");
    tree_free(NULL);

    static const char *const words[] = {"m", "c", "x", "a", "e", "m", "z", "c", "m", "b"};
    for (size_t i = 0; i < sizeof words / sizeof words[0]; i++)
        if (!tree_add(&root, words[i]))
            return oom(root);
    check(tree_count(root, "m") == 3 && tree_count(root, "c") == 2 && tree_count(root, "b") == 1,
          "counts");
    check(tree_count(root, "q") == 0, "absent key");
    check(strcmp(root->key, "m") == 0 && strcmp(root->left->key, "c") == 0
          && strcmp(root->right->key, "x") == 0, "BST shape follows strcmp");

    char key[16];
    for (int i = 0; i < SORTED; i++) {
        snprintf(key, sizeof key, "n%05d", i);     /* all > "m", ascending: a right spine */
        if (!tree_add(&root, key))
            return oom(root);
    }
    snprintf(key, sizeof key, "n%05d", SORTED - 1);
    check(tree_count(root, key) == 1, "deepest key found");
    check(tree_count(root, "n00000") == 1, "first sorted key found");

    tree_free(root);
    return checks_done("m10-e04-tree-free");
}

/* ---- Your definitions of the tree.h functions go below this line. ---- */

bool tree_add(TreeNode **root, const char *key)
{
    TreeNode **pp = root;
    while (*pp) {
        int c = strcmp(key, (*pp)->key);
        if (c == 0) {
            (*pp)->count++;
            return true;
        }
        pp = c < 0 ? &(*pp)->left : &(*pp)->right;
    }
    TreeNode *n = malloc(sizeof *n);
    if (!n)
        return false;
    n->key = strdup(key);
    if (!n->key) {
        free(n);
        return false;
    }
    n->count = 1;
    n->left = n->right = NULL;
    *pp = n;
    return true;
}

int tree_count(const TreeNode *root, const char *key)
{
    while (root) {
        int c = strcmp(key, root->key);
        if (c == 0)
            return root->count;
        root = c < 0 ? root->left : root->right;
    }
    return 0;
}

void tree_free(TreeNode *root)
{
    if (!root)
        return;
    tree_free(root->left);
    tree_free(root->right);
    free(root->key);
    free(root);
}
