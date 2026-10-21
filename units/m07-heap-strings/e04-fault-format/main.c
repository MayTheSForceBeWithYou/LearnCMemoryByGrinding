#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "fmt.h"

int main(void)
{
    char *s = fmt_alloc("%s has %d coins and grade %c", "Ada", 1250, 'A');
    if (s == NULL) {
        fprintf(stderr, "m07-e04-fault-format: out of memory\n");
        return EXIT_OOM;
    }
    check(strcmp(s, "Ada has 1250 coins and grade A") == 0, "fmt_alloc formats");
    free(s);

    const struct player party[] = { { "Ada", 12, 20 }, { "Brigid", 7, 7 }, { "Cormac", 0, 31 } };
    char *r = report_build(party, 3);
    if (r == NULL) {
        fprintf(stderr, "m07-e04-fault-format: out of memory\n");
        return EXIT_OOM;
    }
    check(strcmp(r, "Ada: 12/20 HP\nBrigid: 7/7 HP\nCormac: 0/31 HP\n") == 0, "report text");
    free(r);

    char *e = report_build(party, 0);
    if (e == NULL) {
        fprintf(stderr, "m07-e04-fault-format: out of memory\n");
        return EXIT_OOM;
    }
    check(strcmp(e, "") == 0, "empty report");
    free(e);
    return checks_done("m07-e04-fault-format");
}
