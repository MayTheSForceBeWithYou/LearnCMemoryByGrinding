# M06: Partial failure and cleanup

## Advance organizer

A constructor that makes one allocation has two outcomes. A constructor that makes `k`
allocations has `k + 1`: success, or failure at step 1, step 2, ..., step `k`. At failure
step `j`, allocations `1` through `j - 1` succeeded and must be released, and
allocations `j + 1` through `k` never happened and must **not** be touched. Getting this right
by hand, for every `j`, in every function, is the skill of this unit. `make fault` checks
it mechanically: it fails each allocation in turn and demands a clean exit every time.

Tools in this unit:

- reverse-order cleanup;
- the single-exit `goto cleanup` pattern;
- NULL-initialised locals plus NULL-tolerant frees, so one cleanup block handles every `j`;
- partial-safe destroy, so cleanup can run on a half-built object.

`LearnCwithRPG-terminal` ch10 showed `map_create` failing on its second allocation. This
unit generalises that to any number of steps.

## Foundations

### The failure table

Before writing a multi-step constructor, write its failure table. For

```c
struct db { char *name; int *index; FILE *log; };
```

created in the order name, index, log:

| fails at | already acquired | must release |
|---|---|---|
| `db` struct | nothing | nothing |
| `name` | struct | struct |
| `index` | struct, name | name, struct |
| `log` (`fopen`) | struct, name, index | index, name, struct |

Release in **reverse** order of acquisition. With independent blocks, any order would free
correctly, but reverse order is right when later resources refer to earlier ones (a buffer
registered with a handle, a child that points to its parent), and it's the order a reader
expects.

### Nested ifs: correct, unreadable at three steps

```c
d = malloc(sizeof *d);
if (d) {
    d->name = strdup(name);
    if (d->name) {
        /* ... two more levels ... */
    } else { free(d); d = NULL; }
}
```

It works, and it's a correctness trap once there are four steps and someone adds a fifth.

### The ladder of labels

```c
struct db *db_create(const char *name, size_t n) {
    struct db *d = malloc(sizeof *d);
    if (!d) goto fail_struct;
    d->name = strdup(name);
    if (!d->name) goto fail_name;
    d->index = calloc(n, sizeof *d->index);
    if (!d->index) goto fail_index;
    d->log = fopen("db.log", "a");
    if (!d->log) goto fail_log;
    return d;

fail_log:   free(d->index);
fail_index: free(d->name);
fail_name:  free(d);
fail_struct:
    return NULL;
}
```

Each label frees the resources acquired *before* the step that failed, and falls through
to free the earlier ones. This is the pattern the Linux kernel uses. It's exactly the failure table,
written in code: reading upwards from a label gives the "must release" column.

### Single cleanup with NULL-initialised locals

```c
int load(const char *path, struct result **out) {
    int rc = -1;
    char *buf = NULL;
    int *nums = NULL;
    struct result *r = NULL;

    buf = malloc(BUFSZ);           if (!buf)  goto cleanup;
    nums = malloc(N * sizeof *nums); if (!nums) goto cleanup;
    r = malloc(sizeof *r);          if (!r)    goto cleanup;
    /* ... fill r, transferring nums into it ... */
    r->nums = nums; nums = NULL;    /* transfer: cleanup must not free it */
    *out = r; r = NULL;             /* transfer to caller */
    rc = 0;
cleanup:
    free(r);
    free(nums);
    free(buf);
    return rc;
}
```

One label, and every pointer is freed unconditionally, because the ones never allocated are
NULL and `free(NULL)` is a no-op. The trick that makes success go through the same exit:
**NULL a local the moment ownership moves out of it.** Then cleanup frees only what this function
still owns. Use the ladder when steps have non-`free` teardown with ordering constraints.
Use single cleanup otherwise.

### Partial-safe destroy

If `db_destroy` tolerates NULL fields, then a constructor can use it as its cleanup:
`calloc` the struct (all fields NULL), fill fields one by one, and on any failure call
`db_destroy(d)`. We call this a **partial-safe** destroy: it is safe on any
object whose fields are each either NULL or owned. It is not idempotent: a second call on
the same pointer is a double free, because the struct itself was freed.

## Worked example 1: failure at step 3

For `db_create` above with `FAIL_AT` set so that `calloc` (the third allocation call,
counting `malloc` and `strdup`) fails: the struct block S and the name block N exist.
`d->index` is NULL. Control jumps to `fail_index`, frees N, falls through to `fail_name`,
frees S, and returns NULL. The caller sees NULL and nothing is leaked. `d->log` was never
assigned, and the ladder never reads it.

**Rejected reading:** "Jump to a single `fail:` label that frees everything." At step 3,
`d->log` is uninitialised (the struct came from `malloc`), so `fclose(d->log)` would be
undefined behaviour. The ladder avoids reading fields that were never written. The
single-cleanup style avoids it a different way, by making every field NULL first.

## Worked example 2: the sweep counts calls, not lines

`FAIL_AT=N` counts **allocation calls in execution order**, across the whole run, including
the harness's own. A loop that `strdup`s ten names makes ten calls. So a constructor with
one `malloc` inside a loop has as many failure points as iterations, each with a different
"already acquired" set (the first `i` names). Your cleanup must free exactly the ones
built so far, which means tracking `i`, or NULL-initialising the array with `calloc` and freeing
all slots.

**Rejected reading:** "`fopen` can fail too, so the sweep tests it." The injector wraps the
allocation functions in your object files only. `fopen`'s internal allocation happens inside
libc and is invisible to `--wrap`. Test `fopen` failure separately (a path that can't be
opened). The course's `lib/failalloc` documents this limitation.

## man pointers

- `man 3 strdup`: it allocates, so it counts as an allocation step and the sweep fails it.
- `man 3 fopen`: RETURN VALUE and ERRORS. It's a resource that needs `fclose`, not `free`.
- The kernel's coding style document (`Documentation/process/coding-style.rst`, section
  "Centralized exiting of functions") is the canonical defence of `goto` for cleanup.
  It's not a man page, and optional.

## Check yourself

1. A constructor acquires A, B, C, D in that order and fails acquiring C. What must be
   released, in what order, and what must not be touched?
2. Why does the single-cleanup pattern NULL a local after transferring ownership out of it?
3. Why is `goto fail;` with one label wrong for a `malloc`ed struct whose fields are freed
   at that label?
4. A loop allocates `n` strings into an array and fails at iteration `i`. Which indices hold
   live blocks? Write the condition in C.
5. Why can't the fault sweep test `fopen` failure?

<details><summary>Answers</summary>

1. Release B, then A (reverse order). Don't touch C (it doesn't exist) or D (never acquired).
2. After the transfer the local no longer owns the block. NULLing it makes the shared
   cleanup code's `free` a no-op instead of freeing something the caller now owns.
3. `malloc` leaves the fields indeterminate. A failure before a field is assigned makes the
   label `free` garbage. Use `calloc` (all fields NULL), or one label per stage. And if the
   struct allocation itself failed, the label must not touch its fields at all.
4. Indices `j >= 0 && j < i` hold live blocks. Index `i` failed, and the rest were never assigned.
5. The sweep wraps only the allocation functions. `fopen` fails for other reasons (missing
   file, permissions); you test that path by giving it a file that doesn't exist.

</details>

## Takeaways

- Write the failure table first. The code is a translation of it.
- Release in reverse order of acquisition, and never touch what was never acquired.
- Ladder of labels for ordered teardown. NULL-init plus single cleanup for plain frees.
- NULL a local when ownership leaves it, and cleanup then frees only what you still own.
- A destroy that tolerates half-built objects doubles as constructor cleanup.
- `make fault` fails *every* allocation call in turn. Every run must exit 0 or 3, cleanly.

## Exercises

Every exercise in this unit is graded with the fault sweep, whatever its primary tag.

| # | Folder | Tag | Size | Topic |
|---|---|---|---|---|
| e01 | `e01-two-step/` | `[FAULT]` | S | A two-allocation constructor and destroy; survive both failure points |
| e02 | `e02-leak-on-third/` | `[BUG]` `[FAULT]` | S | A four-step constructor leaks when step 3 fails; find it from the sweep |
| e03 | `e03-goto-cleanup/` | `[FAULT]` | M | A file loader with five resources and a single `goto cleanup` exit |
| e04 | `e04-predict-sweep/` | `[PREDICT]` `[FAULT]` | S | Predict N and the outcome of each `FAIL_AT` for a given constructor, then run the sweep |
| e05 | `e05-idempotent-destroy/` | `[FAULT]` | S | Make `destroy` safe on half-built objects and use it as constructor cleanup |
| e06 | `e06-nested-build/` | `[WRITE]` `[FAULT]` | M | A struct holding an array of owned strings, built in a loop; all-or-nothing |
