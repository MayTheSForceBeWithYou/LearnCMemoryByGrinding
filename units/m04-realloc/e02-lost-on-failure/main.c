#include <string.h>
#include "course.h"
#include "strbuf.h"

static const char *words[] = { "alpha", "beta", "gamma", "delta", "epsilon",
                               "zeta", "eta", "theta", "iota", "kappa" };

int main(void)
{
    struct strbuf sb = {0};

    for (int round = 0; round < 6; round++) {
        for (size_t i = 0; i < sizeof words / sizeof words[0]; i++) {
            if (sb_append(&sb, words[i]) != 0 || sb_append(&sb, " ") != 0) {
                /* A real program might log and carry on: the builder must still be usable. */
                int again = sb_append(&sb, "!");
                check(again == 0 && sb.data[sb.len - 1] == '!', "builder usable after a failed append");
                sb_free(&sb);
                fprintf(stderr, "m04-e02-lost-on-failure: out of memory\n");
                return EXIT_OOM;
            }
        }
    }
    check(sb.len == 6 * 57, "six rounds of ten words and spaces");
    check(strncmp(sb.data, "alpha beta ", 11) == 0, "content starts right");
    check(sb.data[sb.len] == '\0', "NUL terminated");
    sb_free(&sb);
    return checks_done("m04-e02-lost-on-failure");
}
