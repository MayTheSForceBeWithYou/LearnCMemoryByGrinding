#include <string.h>
#include "course.h"
#include "session.h"

int main(void)
{
    struct session *a = session_create("ada", 256, 16);
    struct session *b = session_create("brigid", 1024, 64);
    if (a == NULL || b == NULL) {
        session_destroy(a);
        session_destroy(b);
        fprintf(stderr, "m06-e02-leak-on-third: out of memory\n");
        return EXIT_OOM;
    }
    check(strcmp(a->user, "ada") == 0 && a->bufsize == 256, "session a");
    check(b->histlen == 64 && b->history[63] == 0, "session b history zeroed");
    session_destroy(a);
    session_destroy(b);
    return checks_done("m06-e02-leak-on-third");
}
