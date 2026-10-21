#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "sstack.h"

static int oom(sstack *s)
{
    sstack_destroy(s);
    fprintf(stderr, "m05-e05-design-opaque: out of memory\n");
    return EXIT_OOM;
}

int main(void)
{
    sstack *s = sstack_create();
    if (s == NULL)
        return oom(NULL);
    check(sstack_size(s) == 0 && sstack_pop(s) == NULL && sstack_peek(s) == NULL, "empty stack");

    char word[16];
    for (int i = 0; i < 50; i++) {
        snprintf(word, sizeof word, "w%d", i);
        if (sstack_push(s, word) != 0)
            return oom(s);
    }
    strcpy(word, "changed");                        /* push copied it */
    check(sstack_size(s) == 50, "fifty pushed");
    const char *top = sstack_peek(s);
    check(top != NULL && strcmp(top, "w49") == 0, "peek borrows the top");

    char *p = sstack_pop(s);
    check(p != NULL && strcmp(p, "w49") == 0, "pop transfers the top");
    free(p);
    p = sstack_pop(s);
    check(p != NULL && strcmp(p, "w48") == 0, "LIFO order");
    free(p);
    check(sstack_size(s) == 48, "size after pops");

    sstack_destroy(s);                              /* 48 strings still on it */
    sstack_destroy(NULL);
    return checks_done("m05-e05-design-opaque");
}
