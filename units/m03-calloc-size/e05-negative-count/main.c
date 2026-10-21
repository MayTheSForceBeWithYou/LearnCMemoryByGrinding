#include <stdlib.h>
#include "course.h"
#include "records.h"

#define LE(v) (unsigned char)((v) & 0xff), (unsigned char)(((v) >> 8) & 0xff), \
              (unsigned char)(((v) >> 16) & 0xff), (unsigned char)(((uint32_t)(v) >> 24) & 0xff)

static const unsigned char good[] = { LE(3), LE(1), LE(10), LE(2), LE(20), LE(3), LE(30) };
static const unsigned char big[]  = { LE(2000000000), LE(1), LE(10) };
static const unsigned char neg[]  = { LE(0xffffffffu), LE(1), LE(10), LE(2), LE(20), LE(3), LE(30) };
static const unsigned char shrt[] = { LE(5), LE(1), LE(10) };

int main(void)
{
    size_t n = 0;
    struct record *r = records_parse(good, sizeof good, &n);
    if (r == NULL) {
        fprintf(stderr, "m03-e05-negative-count: good input rejected or out of memory\n");
        return EXIT_OOM;
    }
    check(n == 3 && r[2].value == 30, "good: three records");
    check(r[3].id == 0 && r[3].value == 0, "good: zeroed sentinel");
    free(r);

    check(records_parse(big, sizeof big, &n) == NULL, "big: absurd count rejected");
    check(records_parse(shrt, sizeof shrt, &n) == NULL, "short input rejected without a leak");
    check(records_parse(neg, sizeof neg, &n) == NULL, "neg: negative count rejected");
    return checks_done("m03-e05-negative-count");
}
