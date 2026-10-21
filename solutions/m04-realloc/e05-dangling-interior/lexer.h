#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

struct token {
    size_t off;          /* offset of the first byte in lx->buf: survives any realloc of buf */
    size_t len;
};

/* Owns buf (all text fed so far) and toks. {0} is a valid empty lexer. */
struct lexer {
    char *buf;
    size_t len, cap;
    struct token *toks;
    size_t ntok, tokcap;
};

/* Appends n bytes of text and records every space-separated word in it.
 * Chunks always end on a word boundary. 0 on success, -1 on allocation failure. */
int lexer_feed(struct lexer *lx, const char *chunk, size_t n);
/* Number of tokens so far. */
size_t lexer_count(const struct lexer *lx);
/* Token i's text (not NUL-terminated) and its length in *len.
 * Borrowed: valid until the next lexer_feed or lexer_free
 * (lexer_feed may move the buffer). */
const char *token_text(const struct lexer *lx, size_t i, size_t *len);
/* Frees everything and resets to {0}. */
void lexer_free(struct lexer *lx);

#endif
