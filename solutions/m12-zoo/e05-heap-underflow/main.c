/* m12-e05-heap-underflow [BUG] */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* Removes trailing spaces from the heap string s, in place. */
static void rtrim(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && s[n - 1] == ' ')
        s[--n] = '\0';
}

int main(void)
{
    const char *inputs[] = {"name   ", "  both  ", "none", "     "};
    const char *want[] = {"name", "  both", "none", ""};
    for (int i = 0; i < 4; i++) {
        char *s = strdup(inputs[i]);
        if (!s) {
            fprintf(stderr, "m12-e05: out of memory\n");
            return EXIT_OOM;
        }
        rtrim(s);
        check(strcmp(s, want[i]) == 0, "trailing spaces removed");
        free(s);
    }
    return checks_done("m12-e05-heap-underflow");
}
