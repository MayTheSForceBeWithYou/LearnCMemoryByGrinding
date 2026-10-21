# M02 e02: answers from man 3 malloc

For each: your answer, then the quoted sentence it rests on.

1. What can `malloc(0)` return? Is each possibility safe to pass to `free`?
2. When does `malloc` set `errno`, and to what?
3. What does the page say about overcommit, and what does it mean for a non-NULL result?
4. What does `free` do with NULL? With an already-freed pointer?
5. Which NOTES environment variables affect allocator debugging? What does one of them do?
6. Which functions are C17, which POSIX, and which GNU or BSD?

## Probe observations (all with ARGS=peek: plain; MALLOC_PERTURB_=165; tcache_count=0 plus MALLOC_PERTURB_=165)
