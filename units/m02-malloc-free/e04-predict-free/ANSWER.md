# M02 e04 predictions

| scenario | prediction (defined / glibc abort + message / silent UB) | observed (plain build) | ASan's name for it |
|---|---|---|---|
| 1 free(NULL) | | | |
| 2 free, NULL, free | | | |
| 3 double free | | | |
| 4 double free, malloc between | | | |
| 5 free stack array | | | |
| 6 free p + 1 | | | |

## Surprises
