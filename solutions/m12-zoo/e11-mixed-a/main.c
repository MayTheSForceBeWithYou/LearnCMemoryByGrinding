/* m12-e11-mixed-a [BUG]: two bugs. */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"

typedef struct {
    char *name;   /* owned */
    char *number; /* owned */
} Entry;

typedef struct {
    Entry *items; /* owned array of len entries, room for cap */
    size_t len, cap;
} Book;

/* Appends copies of name and number. Returns 0, or -1 on allocation
 * failure with the book unchanged. */
static int book_add(Book *b, const char *name, const char *number)
{
    if (b->len == b->cap) {
        size_t ncap = b->cap ? b->cap * 2 : 4;
        Entry *p = realloc(b->items, ncap * sizeof *p);
        if (!p)
            return -1;
        b->items = p;
        b->cap = ncap;
    }
    char *n = strdup(name);
    char *num = strdup(number);
    if (!n || !num) {
        free(n);
        free(num);
        return -1;
    }
    b->items[b->len].name = n;
    b->items[b->len].number = num;
    b->len++;
    return 0;
}

/* Removes the entry called name, keeping the order of the others.
 * Returns 1 if one was removed, 0 if there was none. */
static int book_remove(Book *b, const char *name)
{
    for (size_t i = 0; i < b->len; i++) {
        if (strcmp(b->items[i].name, name) == 0) {
            free(b->items[i].name);
            free(b->items[i].number);
            memmove(&b->items[i], &b->items[i + 1], (b->len - i - 1) * sizeof *b->items);
            b->len--;
            return 1;
        }
    }
    return 0;
}

static void book_free(Book *b)
{
    for (size_t i = 0; i < b->len; i++) {
        free(b->items[i].name);
        free(b->items[i].number);
    }
    free(b->items);
    b->items = NULL;
    b->len = b->cap = 0;
}

int main(void)
{
    Book b = {NULL, 0, 0};
    const char *names[] = {"ada", "brian", "carla", "dmitri", "eve"};
    for (int i = 0; i < 5; i++) {
        if (book_add(&b, names[i], "555-0100") != 0) {
            book_free(&b);
            fprintf(stderr, "m12-e11: out of memory\n");
            return EXIT_OOM;
        }
    }
    check(b.len == 5, "five entries");
    check(book_remove(&b, "carla") == 1, "carla removed");
    check(book_remove(&b, "zed") == 0, "zed was never there");
    check(b.len == 4 && strcmp(b.items[2].name, "dmitri") == 0, "order kept");
    book_free(&b);
    return checks_done("m12-e11-mixed-a");
}
