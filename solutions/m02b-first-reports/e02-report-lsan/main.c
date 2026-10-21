#include <stdlib.h>
#include <string.h>
#include "course.h"

struct player {
    char *name;      /* owned heap copy */
    int level;
};

struct roster {
    struct player *players;   /* owned array of cap players */
    size_t count, cap;
};

static struct roster *roster_new(size_t cap)
{
    struct roster *r = malloc(sizeof *r);
    if (r == NULL)
        return NULL;
    r->players = malloc(cap * sizeof *r->players);
    if (r->players == NULL) {
        free(r);
        return NULL;
    }
    r->count = 0;
    r->cap = cap;
    return r;
}

static int roster_add(struct roster *r, const char *name, int level)
{
    if (r->count == r->cap)
        return -1;
    size_t len = strlen(name);
    char *copy = malloc(len + 1);
    if (copy == NULL)
        return -1;
    memcpy(copy, name, len + 1);
    r->players[r->count].name = copy;
    r->players[r->count].level = level;
    r->count++;
    return 0;
}

static void roster_free(struct roster *r)
{
    if (r == NULL)
        return;
    for (size_t i = 0; i < r->count; i++)
        free(r->players[i].name);   /* innermost first: names, */
    free(r->players);               /* then the array that held their pointers, */
    free(r);                        /* then the struct that held the array's pointer */
}

int main(void)
{
    struct roster *r = roster_new(4);
    if (r == NULL) {
        fprintf(stderr, "m02b-e02-report-lsan: out of memory\n");
        return EXIT_OOM;
    }
    const char *names[] = { "Ada", "Brigid", "Cormac" };
    for (int i = 0; i < 3; i++) {
        if (roster_add(r, names[i], i + 1) != 0) {
            roster_free(r);
            fprintf(stderr, "m02b-e02-report-lsan: out of memory\n");
            return EXIT_OOM;
        }
    }
    check(r->count == 3, "three players");
    check(strcmp(r->players[1].name, "Brigid") == 0, "names are copied");

    /* Tear down. */
    roster_free(r);
    return checks_done("m02b-e02-report-lsan");
}
