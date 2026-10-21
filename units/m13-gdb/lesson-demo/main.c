/* M13 lesson demo (not an exercise). Usage: prog grow|watch|asan|uninit */
#include <stdlib.h>
#include <string.h>
#include "course.h"

typedef struct {
    char *data; /* owned */
    size_t len, cap;
} Buf;

static int buf_add(Buf *b, char c)
{
    if (b->len == b->cap) {
        size_t ncap = b->cap ? b->cap * 2 : 4;
        char *p = realloc(b->data, ncap);
        if (!p)
            return -1;
        b->data = p;
        b->cap = ncap;
    }
    b->data[b->len++] = c;
    return 0;
}

static void grow(void)
{
    Buf b = {NULL, 0, 0};
    for (const char *s = "watchpoints"; *s; s++)
        if (buf_add(&b, *s) != 0)
            exit(EXIT_OOM);
    printf("len %zu cap %zu\n", b.len, b.cap);
    free(b.data);
}

static struct {
    int scores[4];
    int round;
} game = {{0, 0, 0, 0}, 1};

static void record(int i, int v)
{
    game.scores[i] = v;
}

static void watch(void)
{
    for (int i = 0; i <= 4; i++)
        record(i, 10 * i);
    printf("round %d\n", game.round);
}

static void asan(void)
{
    int *v = malloc(3 * sizeof *v);
    if (!v)
        exit(EXIT_OOM);
    v[3] = 1;
    free(v);
}

static void uninit(void)
{
    int *v = malloc(2 * sizeof *v);
    if (!v)
        exit(EXIT_OOM);
    v[0] = 7;
    if (v[1] > v[0])
        printf("bigger\n");
    free(v);
}

int main(int argc, char **argv)
{
    const char *m = argc > 1 ? argv[1] : "";
    if (strcmp(m, "grow") == 0)
        grow();
    else if (strcmp(m, "watch") == 0)
        watch();
    else if (strcmp(m, "asan") == 0)
        asan();
    else if (strcmp(m, "uninit") == 0)
        uninit();
    return 0;
}
