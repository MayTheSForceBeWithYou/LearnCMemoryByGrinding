/* M11 lesson demo (not an exercise). Usage: build/.../prog overflow|leaks|uninit */
/* mtrace() is glibc-only; on other platforms (e.g. macOS) it is a no-op and
 * the demo is otherwise unchanged. See docs/macos.md. */
#if defined(__GLIBC__)
#include <mcheck.h>
#define LCMG_MTRACE() mtrace()
#else
#define LCMG_MTRACE() ((void)0)
#endif
#include <stdlib.h>
#include <string.h>
#include "course.h"

typedef struct {
    char *label; /* owned */
} Tag;

static char *config; /* a global root: its block stays reachable */

static void overflow(void)
{
    char *buf = malloc(10);
    if (!buf)
        exit(EXIT_OOM);
    memset(buf, 'a', 10);
    buf[10] = '\0'; /* one past the end */
    free(buf);
}

static void leaks(void)
{
    config = malloc(32);
    Tag *t = malloc(sizeof *t);
    if (!config || !t)
        exit(EXIT_OOM);
    t->label = malloc(48);
    if (!t->label)
        exit(EXIT_OOM);
    t = NULL; /* the Tag and its label are lost */
}

static int sign_of(int x)
{
    return x > 0 ? 1 : (x < 0 ? -1 : 0);
}

static void uninit(void)
{
    int total; /* never initialised */
    for (int i = 0; i < 3; i++)
        if (i > 5)
            total = i;
    printf("sign: %d\n", sign_of(total));
}

int main(int argc, char **argv)
{
    LCMG_MTRACE();
    if (argc > 1 && strcmp(argv[1], "overflow") == 0)
        overflow();
    else if (argc > 1 && strcmp(argv[1], "leaks") == 0)
        leaks();
    else if (argc > 1 && strcmp(argv[1], "uninit") == 0)
        uninit();
    return 0;
}
