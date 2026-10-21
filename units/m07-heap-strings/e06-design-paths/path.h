#ifndef PATH_H
#define PATH_H

/* TODO: ownership contract (who frees, what is borrowed, failure result). */
char       *path_join(const char *dir, const char *file);
/* TODO: ownership contract, including how long the result stays valid. */
const char *path_basename(const char *path);
/* TODO: ownership contract. */
char       *path_dirname(const char *path);
/* TODO: ownership contract. */
char       *path_with_ext(const char *path, const char *ext);

#endif
