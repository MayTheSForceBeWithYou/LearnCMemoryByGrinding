# M02 lesson demo: what `MALLOC_PERTURB_` does and doesn't touch

A probe for the claim in the M02 lesson ("man pointers"): with `MALLOC_PERTURB_` set, glibc fills
blocks with a byte pattern, **but small blocks recycled through the per-thread cache (tcache)
bypass it**.

`main.c` fills a 64-byte block (tcache-sized) with `0x11`, frees it, mallocs the same size again,
and prints bytes 32..47 of what it got back. Then it does the same with a 4096-byte block filled
with `0x22`. That size is above the tcache and fastbin limits and below the mmap threshold, and a
guard allocation keeps it from merging into the top chunk. Bytes 0..31 are skipped because glibc
stores its free-list links there while a block is free.

This reads indeterminate memory on purpose. It is a demo of glibc internals, not a pattern to copy.

## Run it (plain build)

```
make capture TOOL=demo-perturb   # or run the three commands below by hand after `make build`
./build/plain/prog
MALLOC_PERTURB_=170 ./build/plain/prog
GLIBC_TUNABLES=glibc.malloc.tcache_count=0 MALLOC_PERTURB_=170 ./build/plain/prog
```

The value is read as a **decimal** integer (`MALLOC_PERTURB_=0xAA` parses as 0, i.e. off).
Use 170 for 0xAA.

## What to look for

- Unset: both blocks come back at the same address with their old bytes (`11`, `22`).
- `MALLOC_PERTURB_=170`: the 64-byte block **still shows `11`**, because tcache handed it back
  without touching it. The 4096-byte block shows `55`. glibc fills on *allocation* with the
  complement of the byte (`0xAA ^ 0xFF = 0x55`) and on *free* with the byte itself.
- With tcache disabled (`tcache_count=0`), the small block shows `55` too.

`capture-perturb.txt` holds the output from the course machine.
