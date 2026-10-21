/* M01 e03: which of these does the compiler catch?
 * Predict in ANSWER.md before running make. main calls only the valid ones. */
#include <stdio.h>
#include <string.h>
#include "course.h"

struct holder { int *p; };

int *f1(void);
int *f2(void);
struct holder f3(void);
const char *f4(void);
int *f5(void);
int *f6(int *p);

int *f1(void)
{
    int x = 1;
    return &x;
}

int *f2(void)
{
    int x = 2;
    int *p = &x;
    return p;
}

struct holder f3(void)
{
    int x = 3;
    struct holder h = { &x };
    return h;
}

const char *f4(void)
{
    return "four";
}

int *f5(void)
{
    static int x = 5;
    return &x;
}

int *f6(int *p)
{
    return p;
}

int main(void)
{
    int six = 6;

    /* f1, f2 and f3 are never called: their results would be dangling. */
    (void)f1;
    (void)f2;
    (void)f3;
    check(strcmp(f4(), "four") == 0, "f4: string literal has static storage");
    check(*f5() == 5, "f5: static local outlives the call");
    check(f6(&six) == &six && *f6(&six) == 6, "f6: returns the caller's own pointer");
    return checks_done("m01-e03-predict-warning");
}
