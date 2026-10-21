/* m08-e06-copy-rows: deep-copy a string table, all-or-nothing. */
#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"

/* Returns a NULL-terminated table holding a heap copy of each of src[0..n-1].
 * Caller owns the table and the strings; release with free_rows().
 * On allocation failure returns NULL and has freed everything it allocated. */
char **copy_rows(const char *const *src, size_t n);

/* Frees every string, then the table. Accepts NULL. */
void free_rows(char **rows);

int main(void)
{
    static const char *const map[] = {"#######", "#..@..#", "#.~~~.#", "#.....#", "#######"};
    char **empty = copy_rows(map, 0);
    char **copy = empty ? copy_rows(map, 5) : NULL;
    if (!copy) {
        free_rows(empty);
        fprintf(stderr, "m08-e06-copy-rows: out of memory\n");
        return EXIT_OOM;
    }
    check(empty[0] == NULL, "n == 0 gives a table holding only NULL");
    int same = 1;
    for (size_t i = 0; i < 5; i++)
        same &= copy[i] != NULL && copy[i] != map[i] && strcmp(copy[i], map[i]) == 0;
    check(same, "every row copied, not aliased");
    check(copy[5] == NULL, "NULL-terminated");
    copy[1][3] = '.';
    check(map[1][3] == '@', "copy is independent of the source");
    free_rows(copy);
    free_rows(empty);
    free_rows(NULL);
    return checks_done("m08-e06-copy-rows");
}

/* ---- Your definitions of copy_rows and free_rows go below this line. ---- */

char **copy_rows(const char *const *src, size_t n)
{
    if (n > SIZE_MAX / sizeof(char *) - 1)
        return NULL;
    char **rows = malloc((n + 1) * sizeof *rows);
    if (!rows)
        return NULL;
    for (size_t i = 0; i < n; i++) {
        rows[i] = strdup(src[i]);
        if (!rows[i]) {
            while (i-- > 0)                /* rows[0..i-1] exist; the rest is garbage */
                free(rows[i]);
            free(rows);
            return NULL;
        }
    }
    rows[n] = NULL;
    return rows;
}

void free_rows(char **rows)
{
    if (!rows)
        return;
    for (char **r = rows; *r; r++)
        free(*r);
    free(rows);
}
