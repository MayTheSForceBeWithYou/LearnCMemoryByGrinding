#include <string.h>
#include "course.h"
#include "lexer.h"

static int expect(const struct lexer *lx, size_t i, const char *word)
{
    size_t len;
    const char *t = token_text(lx, i, &len);
    return len == strlen(word) && memcmp(t, word, len) == 0;
}

static int run(const char *const *chunks, size_t nchunks, struct lexer *lx)
{
    for (size_t c = 0; c < nchunks; c++)
        if (lexer_feed(lx, chunks[c], strlen(chunks[c])) != 0)
            return -1;
    return 0;
}

int main(void)
{
    static const char *const small[] = { "to be or not" };
    static const char *const large[] = {
        "the quick brown fox ", "jumps over the lazy dog ",
        "and then it ran away into the forest where nobody ",
        "could find it for a very long time indeed, which was the whole point of running ",
    };
    struct lexer a = {0}, b = {0};

    if (run(small, 1, &a) != 0 || run(large, 4, &b) != 0) {
        lexer_free(&a);
        lexer_free(&b);
        fprintf(stderr, "m04-e05-dangling-interior: out of memory\n");
        return EXIT_OOM;
    }
    check(lexer_count(&a) == 4 && expect(&a, 0, "to") && expect(&a, 3, "not"), "small input");
    check(lexer_count(&b) == 35, "large input: 35 words");
    check(expect(&b, 0, "the") && expect(&b, 3, "fox"), "first chunk's tokens after the buffer grew");
    check(expect(&b, 34, "running"), "last token");
    lexer_free(&a);
    lexer_free(&b);
    return checks_done("m04-e05-dangling-interior");
}
