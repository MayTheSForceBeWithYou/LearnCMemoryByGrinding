#define _POSIX_C_SOURCE 200809L     /* for the real strdup/strndup used as references */
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "mystr.h"

static int same(char *mine, char *ref)
{
    int ok = mine != NULL && ref != NULL && strcmp(mine, ref) == 0;
    free(mine);
    free(ref);
    return ok;
}

int main(void)
{
    const char *inputs[] = { "", "a", "hello", "with spaces and punctuation!" };
    for (size_t i = 0; i < sizeof inputs / sizeof inputs[0]; i++) {
        check(same(my_strdup(inputs[i]), strdup(inputs[i])), "my_strdup matches strdup");
        size_t ns[] = { 0, 1, 3, 100 };
        for (size_t k = 0; k < 4; k++)
            check(same(my_strndup(inputs[i], ns[k]), strndup(inputs[i], ns[k])), "my_strndup matches strndup");
    }

    char *s = my_strdup("independent");
    if (s == NULL) {
        fprintf(stderr, "m07-e01-my-strdup: out of memory\n");
        return EXIT_OOM;
    }
    check(s != (const char *)"independent", "a copy, not the literal");
    s[0] = 'I';                              /* writable: it is ours */
    check(strcmp(s, "Independent") == 0, "copy is writable");
    free(s);

    /* Exactly 4 bytes, no NUL anywhere: reading s[4] would be an overflow ASan reports. */
    char *raw = malloc(4);
    if (raw == NULL) {
        fprintf(stderr, "m07-e01-my-strdup: out of memory\n");
        return EXIT_OOM;
    }
    memcpy(raw, "abcd", 4);
    char *t = my_strndup(raw, 4);
    check(t != NULL && strcmp(t, "abcd") == 0, "unterminated source, n == size");
    free(t);
    free(raw);
    return checks_done("m07-e01-my-strdup");
}
