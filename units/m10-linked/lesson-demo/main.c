/* m10 lesson demo: recursive post-order free of a degenerate tree (what sorted keys build).
 * Usage: prog N  -- builds an N-node right spine with owned string keys, then frees it
 * recursively, so the recursion is N frames deep. Quoted in ../LESSON.md. */
#define _POSIX_C_SOURCE 200809L    /* strdup() under -std=c17; before any #include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TreeNode {
    char            *key;
    int              count;
    struct TreeNode *left, *right;
} TreeNode;

static void tree_free(TreeNode *t)
{
    if (!t)
        return;
    tree_free(t->left);
    tree_free(t->right);
    free(t->key);
    free(t);
}

int main(int argc, char **argv)
{
    long n = argc > 1 ? strtol(argv[1], NULL, 10) : 1000;
    TreeNode *root = NULL, **link = &root;
    char key[32];
    for (long i = 0; i < n; i++) {             /* iterative build: each key larger than the last */
        TreeNode *t = calloc(1, sizeof *t);
        snprintf(key, sizeof key, "k%09ld", i);
        if (!t || !(t->key = strdup(key))) {
            free(t);
            tree_free(root);
            fprintf(stderr, "out of memory\n");
            return 3;
        }
        t->count = 1;
        *link = t;
        link = &t->right;
    }
    printf("built a degenerate tree of %ld nodes; freeing it recursively\n", n);
    fflush(stdout);
    tree_free(root);
    printf("freed %ld nodes\n", n);
    return 0;
}
