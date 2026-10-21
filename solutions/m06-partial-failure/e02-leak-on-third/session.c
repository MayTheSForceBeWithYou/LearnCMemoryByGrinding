#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "session.h"

struct session *session_create(const char *user, size_t bufsize, size_t histlen)
{
    struct session *s = malloc(sizeof *s);
    if (s == NULL)
        goto fail_struct;
    s->user = strdup(user);
    if (s->user == NULL)
        goto fail_user;
    s->buf = malloc(bufsize);
    if (s->buf == NULL)
        goto fail_buf;
    s->history = calloc(histlen, sizeof *s->history);
    if (s->history == NULL)
        goto fail_history;
    s->bufsize = bufsize;
    s->histlen = histlen;
    return s;

fail_history:
    free(s->buf);
fail_buf:
    free(s->user);
fail_user:
    free(s);
fail_struct:
    return NULL;
}

void session_destroy(struct session *s)
{
    if (s == NULL)
        return;
    free(s->history);
    free(s->buf);
    free(s->user);
    free(s);
}
