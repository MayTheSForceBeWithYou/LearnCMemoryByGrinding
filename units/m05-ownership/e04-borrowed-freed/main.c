#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "roster.h"

int main(void)
{
    struct roster *r = roster_create();
    if (r == NULL) {
        fprintf(stderr, "m05-e04-borrowed-freed: out of memory\n");
        return EXIT_OOM;
    }
    const char *input[] = { "ada", "brigid", "cormac", "dana", "eamon" };
    for (size_t i = 0; i < 5; i++) {
        char *tmp = strdup(input[i]);            /* e.g. a line we just read */
        if (tmp == NULL || roster_add(r, tmp) != 0) {
            free(tmp);
            roster_destroy(r);
            fprintf(stderr, "m05-e04-borrowed-freed: out of memory\n");
            return EXIT_OOM;
        }
        free(tmp);                               /* legal: roster_add copies (see roster.h) */
    }
    check(roster_size(r) == 5, "five players");

    const char *first = roster_name(r, 0);
    check(strcmp(first, "ada") == 0, "first name");
    free((char *)first);                         /* done with it */
    check(strcmp(roster_name(r, 4), "eamon") == 0, "last name");
    roster_destroy(r);
    return checks_done("m05-e04-borrowed-freed");
}
