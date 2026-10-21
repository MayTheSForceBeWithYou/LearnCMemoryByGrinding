# M07 e05 notes

Expected output (x86-64, glibc):

```
 1: sizeof arr          = 6
 2: strlen(arr)         = 5
 3: sizeof ptr          = 8
 4: sizeof "hi"         = 3
 5: strlen(strndup(arr,  0)) = 0
 6: strlen(strndup(arr,  3)) = 3
 7: strlen(strndup(arr, 99)) = 5
 8: snprintf return     = 6
 9: buf now holds       = "abc"
10: strlen(buf)         = 3
11: snprintf(NULL, 0, "%d", -1234) = 5
12: sizeof buf - strlen(buf) = 1
m07-e05-predict-lengths: all checks passed
```

Rules:
- `sizeof` an array counts the NUL. `sizeof` a pointer is the pointer's size. A literal is an array.
- `strndup` copies at most `n` bytes and always terminates. It never reads past the source's NUL.
- `snprintf` returns the length it *would* have written, which is how you detect truncation (`r >= sizeof buf`). It always writes a
  NUL when size > 0, so a 4-byte buffer holds 3 characters.
- The minus sign counts toward the length.
