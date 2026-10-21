/* M01 e04 harness. Do not edit. */
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "pos.h"

int main(void)
{
    /* static */
    const char *a = pos_static(3, -12);
    check(strcmp(a, "(3, -12)") == 0, "pos_static formats");
    char saved[32];
    strcpy(saved, a);
    const char *b = pos_static(1, 2);
    check(strcmp(b, "(1, 2)") == 0, "pos_static second call");
    check(a == b, "pos_static returns the same buffer every time");
    check(strcmp(saved, "(3, -12)") == 0, "copying out is how you keep a static result");
    check(strcmp(pos_static(INT_MIN, INT_MAX), "(-2147483648, 2147483647)") == 0, "pos_static INT_MIN");

    /* caller buffer */
    char buf[32];
    check(pos_into(buf, sizeof buf, 7, 8) == buf, "pos_into returns dst");
    check(strcmp(buf, "(7, 8)") == 0, "pos_into formats");
    char small[5];
    memset(small, 'Z', sizeof small);
    check(pos_into(small, sizeof small, 100, 200) == NULL, "pos_into reports truncation");
    check(memchr(small, '\0', sizeof small) != NULL, "truncated result is still a string");
    char untouched = 'Q';
    check(pos_into(&untouched, 0, 1, 1) == NULL && untouched == 'Q', "cap == 0 writes nothing");
    char exact[7]; /* "(7, 8)" is 6 chars + NUL */
    check(pos_into(exact, sizeof exact, 7, 8) == exact, "exact fit is not truncation");
    char big[32];
    check(pos_into(big, sizeof big, INT_MIN, INT_MIN) != NULL &&
          strcmp(big, "(-2147483648, -2147483648)") == 0, "pos_into INT_MIN");

    /* heap: two results alive at once */
    char *h1 = pos_heap(-1, -1);
    char *h2 = pos_heap(INT_MIN, 0);
    if (h1 == NULL || h2 == NULL) {
        free(h1);
        free(h2);
        fprintf(stderr, "m01-e04-three-ways: out of memory\n");
        return EXIT_OOM;
    }
    check(h1 != h2, "pos_heap gives independent blocks");
    check(strcmp(h1, "(-1, -1)") == 0, "pos_heap formats");
    check(strcmp(h2, "(-2147483648, 0)") == 0, "pos_heap INT_MIN, still valid after a second call");
    free(h1);
    free(h2);
    return checks_done("m01-e04-three-ways");
}
