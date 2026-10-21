#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "label.h"

int main(void)
{
    char *a = label_make("Ada", 7);
    char *b = label_upper("brigid", 42);
    char *c = label_make("", 1000);
    if (a == NULL || b == NULL || c == NULL) {
        free(a);
        free(b);
        free(c);
        fprintf(stderr, "m07-e02-off-by-one-nul: out of memory\n");
        return EXIT_OOM;
    }
    check(strcmp(a, "[Lv 7] Ada!") == 0, "label_make");
    check(strcmp(b, "[Lv 42] BRIGID!") == 0, "label_upper");
    check(strcmp(c, "[Lv 1000] !") == 0, "empty name");
    free(a);
    free(b);
    free(c);
    return checks_done("m07-e02-off-by-one-nul");
}
