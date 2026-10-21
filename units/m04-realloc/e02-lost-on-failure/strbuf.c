#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "strbuf.h"

int sb_append(struct strbuf *sb, const char *s)
{
    size_t n = strlen(s);

    if (sb->len + n + 1 > sb->cap) {
        size_t newcap = sb->cap ? sb->cap : 16;
        while (newcap < sb->len + n + 1) {
            if (newcap > SIZE_MAX / 2)
                return -1;
            newcap *= 2;
        }
        sb->cap = newcap;
        sb->data = realloc(sb->data, newcap);
        if (sb->data == NULL)
            return -1;
    }
    memcpy(sb->data + sb->len, s, n + 1);
    sb->len += n;
    return 0;
}

void sb_free(struct strbuf *sb)
{
    free(sb->data);
    sb->data = NULL;
    sb->len = sb->cap = 0;
}
