/* M03 e04: predict each printed value in ANSWER.md before running. Do not edit.
 * This exercise builds with -Wconversion -Wsign-conversion: read the warnings too. */
#include <stdint.h>
#include <stdio.h>
#include "course.h"

int main(void)
{
    size_t n = 0;
    uint32_t w = 65536, h = 65536;
    unsigned u = 1;
    int i = -1;
    int big = 2147483647;
    short s = -1;

    printf(" 1: %zu\n", (size_t)-1);
    printf(" 2: %zu\n", -1 * sizeof(int));
    printf(" 3: %d\n", (int)3000000000u);
    printf(" 4: %zu\n", n - 1);
    printf(" 5: %zu\n", (size_t)(w * h * 4));
    printf(" 6: %zu\n", (size_t)w * h * 4);
    printf(" 7: %d\n", i < u);
    printf(" 8: %zu\n", (size_t)big + 1);
    printf(" 9: %u\n", (unsigned)s);
    printf("10: %zu\n", sizeof(int) - 5);

    check(1, "this program only prints");
    return checks_done("m03-e04-predict-conversions");
}
