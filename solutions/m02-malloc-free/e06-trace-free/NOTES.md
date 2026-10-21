# M02 e06 notes

| mark | p | q | r | B1 (24 B) | B2 (16 B) |
|---|---|---|---|---|---|
| 1 | -> B1 | -> B1 | NULL | live, 2 refs | does not exist |
| 2 | NULL | dangling (B1) | -> B2 | **freed** | live, 1 ref |
| 3 | NULL | -> B2 | -> B2 | freed | live, 2 refs |
| 4 | NULL | -> B2 | NULL | freed | live, 1 ref |
| 5 | NULL | NULL | NULL | freed | live, **0 refs, leaked** |

- B2 first becomes unreachable at **TRACE 5**, when `q`, the last reference, is overwritten. At mark 4 it's still reachable
  through `q`. Students often say mark 4, because they track "the variable that received the `malloc`" instead of
  "any variable holding the address".
- At mark 2, `q` still holds B1's address. The printout says `B1-addr`, which shows that an address value survives `free`. The
  program avoids reading `q` itself after the free by printing the copy `qv`, made beforehand.
- B1 is not leaked: it was freed. Dangling and leaked are opposite failures (pointer without block, block without pointer).
