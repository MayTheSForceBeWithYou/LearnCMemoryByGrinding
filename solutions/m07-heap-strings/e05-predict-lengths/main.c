#define _POSIX_C_SOURCE 200809L   /* strndup is POSIX, not ISO C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"

int main(void)
{
    char arr[] = "hello";
    const char *ptr = arr;
    char buf[4];
    static char src[] = "abcdef";

    printf(" 1: sizeof arr          = %zu\n", sizeof arr);
    printf(" 2: strlen(arr)         = %zu\n", strlen(arr));
    printf(" 3: sizeof ptr          = %zu\n", sizeof ptr);
    printf(" 4: sizeof \"hi\"         = %zu\n", sizeof "hi");

    size_t ns[] = { 0, 3, 99 };
    for (int i = 0; i < 3; i++) {
        char *d = strndup(arr, ns[i]);
        if (d == NULL) {
            fprintf(stderr, "m07-e05-predict-lengths: out of memory\n");
            return EXIT_OOM;
        }
        printf(" %d: strlen(strndup(arr, %2zu)) = %zu\n", 5 + i, ns[i], strlen(d));
        free(d);
    }

    int r = snprintf(buf, sizeof buf, "%s", src);
    printf(" 8: snprintf return     = %d\n", r);
    printf(" 9: buf now holds       = \"%s\"\n", buf);
    printf("10: strlen(buf)         = %zu\n", strlen(buf));
    printf("11: snprintf(NULL, 0, \"%%d\", -1234) = %d\n", snprintf(NULL, 0, "%d", -1234));
    printf("12: sizeof buf - strlen(buf) = %zu\n", sizeof buf - strlen(buf));
    return checks_done("m07-e05-predict-lengths");
}
