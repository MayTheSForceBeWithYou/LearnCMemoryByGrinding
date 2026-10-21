#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "pair.h"

int main(void)
{
    static struct pair sentinel_obj;
    struct pair *const SENTINEL = &sentinel_obj;
    struct pair *p = SENTINEL;

    int rc = parse_pair("speed=fast", &p);
    if (rc != 0 && p == SENTINEL) {
        /* could only be OOM for this valid line */
        fprintf(stderr, "m05-e03-out-param: out of memory\n");
        return EXIT_OOM;
    }
    check(rc == 0 && p != SENTINEL, "valid line parsed");
    check(strcmp(p->key, "speed") == 0 && strcmp(p->value, "fast") == 0, "key and value");
    pair_destroy(p);

    char line[] = "a=";
    p = SENTINEL;
    rc = parse_pair(line, &p);
    if (rc != 0) {
        fprintf(stderr, "m05-e03-out-param: out of memory\n");
        return EXIT_OOM;
    }
    line[0] = 'Z';                          /* line was only borrowed */
    check(strcmp(p->key, "a") == 0 && strcmp(p->value, "") == 0, "empty value is valid, and copied");
    pair_destroy(p);

    const char *bad[] = { "novalue", "=b", "", "==" };
    for (size_t i = 0; i < sizeof bad / sizeof bad[0]; i++) {
        p = SENTINEL;
        check(parse_pair(bad[i], &p) == -1 && p == SENTINEL, "malformed: -1 and *out untouched");
    }
    p = SENTINEL;
    check(parse_pair("k=v=w", &p) == 0 && strcmp(p->value, "v=w") == 0, "split at the first =");
    if (p != SENTINEL)
        pair_destroy(p);
    pair_destroy(NULL);
    return checks_done("m05-e03-out-param");
}
