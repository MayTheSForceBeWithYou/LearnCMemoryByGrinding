# M05 e01 notes

| site | kind | frees | valid until |
|---|---|---|---|
| 1 `cfg_new` | owned | caller, `cfg_free` | `cfg_free` |
| 2 `cfg_get` | borrowed from `c` | nobody | `cfg_reload` / `cfg_free` |
| 3 `cfg_dup` | owned | caller, `free` | its `free` |
| 4 `cfg_path` | borrowed from `c` | nobody | `cfg_free` |
| 5 `names_default` | literal | nobody | forever |
| 6 `names_join` | owned | caller, `free` | its `free` |
| 7 `names_upper` | static | nobody | the next `names_upper` call |
| 8 `names_pick` | borrowed from the caller's own array | nobody | while `party` is in scope |
| 9 `fmt_int` | static | nobody | the next `fmt_int` |
| 10 `fmt_into` | caller's storage (`lbuf`) | nobody | `lbuf`'s scope |
| 11 `fmt_hex_new` | owned | caller, `free` | its `free` |
| 12 `fmt_version` | literal | nobody | forever |

- **STATE A:** heap blocks are the cfg struct (owner `main` via `c`), and inside it `path`, `name`, `mode` (owner: the cfg), `name_copy`
  (`main`), `full` (`main`), and `hex` (`main`). `name` and `path` borrow into the cfg. Nothing dangles.
- **STATE B:** `cfg_reload` freed the old `name` and `mode` blocks and made new ones, so **`name` dangles**. The program never reads it
  after the reload. That's legal, and it's exactly the trap. `path` is still valid, because reload doesn't rebuild it. `name_copy` is unaffected (owned).
- Sites 7 and 9 are the ones people label "owned", because "it returned a `char *` I didn't pass in". Read the body: `static char buf`.
- Site 8 is the subtle one. `names_pick` returns a pointer it never owned, and its validity is the *caller's* own array's.
