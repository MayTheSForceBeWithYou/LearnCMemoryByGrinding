/* M02b e03: predict Valgrind's summary before running make valgrind. */
#include <stdlib.h>
#include <string.h>
#include "course.h"

struct pair {
    char *left;
    char *right;
};

static char *config_cache;   /* a global holding a heap pointer */

int main(void)
{
    struct pair *p = malloc(sizeof *p);          /* 16 bytes */
    char *scratch = malloc(100);
    char *tmp = malloc(10);
    config_cache = malloc(32);
    if (p == NULL || scratch == NULL || tmp == NULL || config_cache == NULL) {
        free(p); free(scratch); free(tmp); free(config_cache);
        fprintf(stderr, "m02b-e03-predict-valgrind: out of memory\n");
        return EXIT_OOM;
    }
    p->left = malloc(40);
    p->right = malloc(24);
    if (p->left == NULL || p->right == NULL) {
        free(p->left); free(p->right); free(p); free(scratch); free(tmp); free(config_cache);
        fprintf(stderr, "m02b-e03-predict-valgrind: out of memory\n");
        return EXIT_OOM;
    }
    strcpy(p->left, "left");
    strcpy(p->right, "right");
    memset(scratch, 0, 100);
    strcpy(config_cache, "mode=fast");
    strcpy(tmp, "temp");

    check(strcmp(p->left, "left") == 0, "pair is filled");
    free(tmp);
    p = NULL;          /* "done with it" */
    scratch = NULL;
    return checks_done("m02b-e03-predict-valgrind");
}
