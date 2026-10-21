# M05 e02: Write the contract first `[DESIGN]`

## Goal

`contact.h` declares an API for an address-book entry, with **no comments**:

```c
struct contact { char *name; char *email; char **tags; size_t ntags, cap; };

struct contact *contact_create(const char *name, const char *email);
void            contact_destroy(struct contact *c);
int             contact_add_tag(struct contact *c, const char *tag);
const char     *contact_name(const struct contact *c);
int             contact_set_email(struct contact *c, const char *email);
char           *contact_to_string(const struct contact *c);
```

Part 1: write the ownership contract above every declaration. For each one, say who owns
the result, what happens to each pointer argument (copied, borrowed for the call, or
retained), how long borrowed results stay valid, and the failure behaviour.

Part 2: implement `contact.c` so that it honours exactly what you wrote. The harness in
`main.c` assumes the course conventions from the lesson (copy inputs, destroy accepts NULL,
setters leave the object unchanged on failure). Where your contract and the harness
disagree, the harness wins. Adjust the contract.

**The starter will not build until you write `contact.c`.** The link error is intentional.

## Constraints

- `contact_name`'s result must stay valid across `contact_add_tag` calls. Think about where
  the name lives.
- `contact_set_email` on allocation failure leaves the old email in place.
- `contact_to_string` returns a heap string the caller frees. Size it exactly.
- No leaks, including on every failure path. This unit isn't `[FAULT]`-graded yet (M06
  is), but `make fault` is available, and your code will be reused there.

## How to verify

- `make check`: checks, ASan, and Valgrind clean.
- Your header comments are reviewed against the solution's NOTES.md for completeness.
