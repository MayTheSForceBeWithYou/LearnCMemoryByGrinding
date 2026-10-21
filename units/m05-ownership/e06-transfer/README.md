# M05 e06: Taking ownership `[WRITE]`

## Goal

Implement a message queue that takes ownership of heap strings pushed onto it:

```c
int   queue_push_owned(struct queue *q, char *msg);   /* takes ownership on success */
char *queue_pop(struct queue *q);                     /* caller owns the result */
void  queue_clear(struct queue *q);                   /* frees all queued messages */
```

`struct queue` is a complete type in `queue.h` (a growable ring or array: your choice of
internals within the fields provided). The harness pushes `strdup`'d messages, pops some,
clears, and checks that every message is freed exactly once.

**The starter will not build until you write the three definitions.** The link error is intentional.

## Constraints

- The comment in `queue.h` already states the failure contract: **on failure, ownership of
  `msg` stays with the caller.** The harness frees `msg` itself when `queue_push_owned` returns -1.
  Your code must not free it on that path, and must not have stored it.
- After a successful push, the caller doesn't touch `msg` again. The harness checks that
  popping gives back the same address it pushed.
- `queue_clear` leaves the queue empty and reusable.

## How to verify

- `make check`: checks, ASan, and Valgrind clean.
- `make fault` is not part of `make check` here, but it should pass. Try it.
