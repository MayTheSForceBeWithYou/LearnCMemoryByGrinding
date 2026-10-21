/* m10-e02-free-loop: a playlist as a linked list. One memory bug. */
#define _POSIX_C_SOURCE 200809L    /* strdup() under -std=c17; before any #include */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"

typedef struct Track {
    char         *title;     /* owned */
    char         *artist;    /* owned */
    unsigned      seconds;
    struct Track *next;
} Track;

static void playlist_destroy(Track *head);

static bool playlist_add(Track **tail_link, const char *title, const char *artist, unsigned seconds)
{
    Track *t = calloc(1, sizeof *t);
    if (!t)
        return false;
    t->title = strdup(title);
    t->artist = strdup(artist);
    if (!t->title || !t->artist) {
        playlist_destroy(t);
        return false;
    }
    t->seconds = seconds;
    *tail_link = t;
    return true;
}

static void playlist_destroy(Track *head)
{
    Track *t = head;
    while (t != NULL) {
        Track *next = t->next;     /* read before t is freed */
        free(t->title);
        free(t->artist);
        free(t);
        t = next;
    }
}

int main(void)
{
    static const struct { const char *title, *artist; unsigned seconds; } songs[] = {
        {"Blue in Green", "Miles Davis", 337},
        {"Naima", "John Coltrane", 261},
        {"Peace Piece", "Bill Evans", 402},
        {"Footprints", "Wayne Shorter", 441},
    };
    Track *head = NULL, **link = &head;
    for (size_t i = 0; i < sizeof songs / sizeof songs[0]; i++) {
        if (!playlist_add(link, songs[i].title, songs[i].artist, songs[i].seconds)) {
            playlist_destroy(head);
            fprintf(stderr, "m10-e02-free-loop: out of memory\n");
            return EXIT_OOM;
        }
        link = &(*link)->next;
    }

    unsigned total = 0, count = 0;
    for (const Track *t = head; t; t = t->next) {
        printf("%-14s %-14s %u:%02u\n", t->title, t->artist, t->seconds / 60, t->seconds % 60);
        total += t->seconds;
        count++;
    }
    check(count == 4, "four tracks");
    check(total == 1441, "total running time");
    playlist_destroy(head);
    return checks_done("m10-e02-free-loop");
}
