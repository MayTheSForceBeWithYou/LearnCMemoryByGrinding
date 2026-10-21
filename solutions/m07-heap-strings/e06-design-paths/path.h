#ifndef PATH_H
#define PATH_H

/* Returns a new string "dir/file" with exactly one '/' between them (none if
 * dir is ""). Caller frees. dir and file are borrowed for the call only.
 * NULL on allocation failure. */
char       *path_join(const char *dir, const char *file);
/* Returns a pointer INTO path: the part after the last '/'. Allocates nothing;
 * do not free it. Valid only while path is alive and unmodified. */
const char *path_basename(const char *path);
/* Returns a new string: everything before the last '/', "." if there is none,
 * "/" if that slash is the first character. Caller frees. NULL on failure. */
char       *path_dirname(const char *path);
/* Returns a new string: path with its basename's last extension replaced by
 * ext (or ext appended if there is none). Caller frees. NULL on failure. */
char       *path_with_ext(const char *path, const char *ext);

#endif
