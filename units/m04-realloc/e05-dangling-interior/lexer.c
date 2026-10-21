#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

static int reserve(void **p, size_t *cap, size_t need, size_t elem)
{
    if (need <= *cap)
        return 0;
    size_t nc = *cap ? *cap : 16;
    while (nc < need) {
        if (nc > SIZE_MAX / 2 / elem)
            return -1;
        nc *= 2;
    }
    void *tmp = realloc(*p, nc * elem);
    if (tmp == NULL)
        return -1;
    *p = tmp;
    *cap = nc;
    return 0;
}

int lexer_feed(struct lexer *lx, const char *chunk, size_t n)
{
    void *b = lx->buf;
    if (reserve(&b, &lx->cap, lx->len + n, 1) != 0)
        return -1;
    lx->buf = b;
    size_t base = lx->len;
    memcpy(lx->buf + base, chunk, n);
    lx->len += n;

    size_t i = base;
    while (i < lx->len) {
        while (i < lx->len && lx->buf[i] == ' ')
            i++;
        size_t s = i;
        while (i < lx->len && lx->buf[i] != ' ')
            i++;
        if (i > s) {
            void *t = lx->toks;
            if (reserve(&t, &lx->tokcap, lx->ntok + 1, sizeof *lx->toks) != 0)
                return -1;
            lx->toks = t;
            lx->toks[lx->ntok].start = lx->buf + s;
            lx->toks[lx->ntok].len = i - s;
            lx->ntok++;
        }
    }
    return 0;
}

size_t lexer_count(const struct lexer *lx)
{
    return lx->ntok;
}

const char *token_text(const struct lexer *lx, size_t i, size_t *len)
{
    *len = lx->toks[i].len;
    return lx->toks[i].start;
}

void lexer_free(struct lexer *lx)
{
    free(lx->buf);
    free(lx->toks);
    memset(lx, 0, sizeof *lx);
}
