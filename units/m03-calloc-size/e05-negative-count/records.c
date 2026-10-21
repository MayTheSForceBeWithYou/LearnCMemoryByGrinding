#include <stdlib.h>
#include <string.h>
#include "records.h"

static int32_t rd32(const unsigned char *p)
{
    uint32_t u = (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
    return (int32_t)u;
}

struct record *records_parse(const unsigned char *buf, size_t len, size_t *out_n)
{
    if (len < 4)
        return NULL;
    int32_t count = rd32(buf);
    if (count > RECORDS_MAX)
        return NULL;
    struct record *r = malloc((size_t)(count + 1) * sizeof *r);
    if (r == NULL)
        return NULL;

    size_t off = 4;
    int32_t i = 0;
    for (; i != count && off + 8 <= len; i++, off += 8) {
        r[i].id = rd32(buf + off);
        r[i].value = rd32(buf + off + 4);
    }
    if (i != count) {          /* input shorter than the header claims */
        free(r);
        return NULL;
    }
    memset(&r[count], 0, sizeof r[count]);
    *out_n = (size_t)count;
    return r;
}
