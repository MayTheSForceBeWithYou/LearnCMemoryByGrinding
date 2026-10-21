#ifndef SESSION_H
#define SESSION_H

#include <stddef.h>

struct session {
    char *user;          /* owned copy */
    unsigned char *buf;  /* bufsize bytes */
    size_t bufsize;
    long *history;       /* histlen zeroed entries */
    size_t histlen;
};

/* New session; caller owns it (session_destroy). NULL on allocation failure, nothing leaked. */
struct session *session_create(const char *user, size_t bufsize, size_t histlen);
/* Accepts NULL. */
void session_destroy(struct session *s);

#endif
