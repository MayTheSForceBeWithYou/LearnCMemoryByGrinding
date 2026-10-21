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
