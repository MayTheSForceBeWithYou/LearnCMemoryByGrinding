#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "cfg.h"
#include "names.h"
#include "fmt.h"

int main(void)
{
    struct cfg *c = cfg_new("game.cfg");                       /* WHO-1 */
    if (c == NULL) {
        fprintf(stderr, "m05-e01-who-frees-this: out of memory\n");
        return EXIT_OOM;
    }
    const char *name = cfg_get(c, "name");                      /* WHO-2 */
    char *name_copy = cfg_dup(c, "name");                       /* WHO-3 */
    const char *path = cfg_path(c);                             /* WHO-4 */
    const char *dflt = names_default();                         /* WHO-5 */
    char *full = names_join(name, "the brave");                 /* WHO-6 */
    if (name_copy == NULL || full == NULL) {
        free(name_copy);
        free(full);
        cfg_free(c);
        fprintf(stderr, "m05-e01-who-frees-this: out of memory\n");
        return EXIT_OOM;
    }
    check(strcmp(names_upper(name), "HERO") == 0, "upper");    /* WHO-7 */
    const char *const party[] = { "ada", "brigid" };
    const char *second = names_pick(party, 1);                  /* WHO-8 */
    const char *lvl = fmt_int(7);                               /* WHO-9 */
    char lbuf[16];
    char *lvl2 = fmt_into(lbuf, sizeof lbuf, 8);                /* WHO-10 */
    char *hex = fmt_hex_new(255);                               /* WHO-11 */
    const char *ver = fmt_version();                            /* WHO-12 */
    if (hex == NULL) {
        free(name_copy);
        free(full);
        cfg_free(c);
        fprintf(stderr, "m05-e01-who-frees-this: out of memory\n");
        return EXIT_OOM;
    }
    /* STATE A */
    check(strcmp(name, "hero") == 0 && strcmp(path, "game.cfg") == 0, "borrows read before reload");
    check(strcmp(full, "hero the brave") == 0 && strcmp(dflt, "nobody") == 0, "join and default");
    check(strcmp(second, "brigid") == 0 && strcmp(lvl, "7") == 0, "pick and int");
    check(lvl2 == lbuf && strcmp(lvl2, "8") == 0, "into");
    check(strcmp(hex, "0xff") == 0 && strcmp(ver, "1.4.2") == 0, "hex and version");

    if (cfg_reload(c) != 0) {
        free(name_copy);
        free(full);
        free(hex);
        cfg_free(c);
        fprintf(stderr, "m05-e01-who-frees-this: out of memory\n");
        return EXIT_OOM;
    }
    /* STATE B */
    check(strcmp(name_copy, "hero") == 0, "the owned copy survives the reload");
    check(strcmp(cfg_get(c, "name"), "hero-v2") == 0, "a fresh borrow sees the new value");
    check(strcmp(path, "game.cfg") == 0, "path is not rebuilt by reload");

    free(hex);
    free(full);
    free(name_copy);
    cfg_free(c);
    return checks_done("m05-e01-who-frees-this");
}
