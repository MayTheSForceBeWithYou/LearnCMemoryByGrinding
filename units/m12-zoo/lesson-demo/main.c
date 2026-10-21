/* M12 lesson demo (not an exercise). Usage: prog [read|tcache] */
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* read: one int read just past a 4-int block. */
static int read_past(void)
{
    int *v = malloc(4 * sizeof *v);
    if (!v)
        exit(EXIT_OOM);
    for (int i = 0; i < 4; i++)
        v[i] = i;
    int sum = 0;
    for (int i = 0; i <= 4; i++)
        sum += v[i];
    free(v);
    return sum;
}

/* tcache: the same small block freed twice in a row. */
static void free_twice(void)
{
    char *p = malloc(32);
    if (!p)
        exit(EXIT_OOM);
    free(p);
    free(p);
}

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "read") == 0)
        printf("sum %d\n", read_past());
    else if (argc > 1 && strcmp(argv[1], "tcache") == 0)
        free_twice();
    return 0;
}
