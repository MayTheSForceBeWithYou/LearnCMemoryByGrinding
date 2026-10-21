# m09-e04-held-pointer `[REPORT]`

## Goal

`main.c` keeps a running "current best" while it fills a vector, and keeps it as a pointer
into the vector's buffer. Below is the ASan report from running the shipped source with `make asan`.

**From the report and the source alone, before running anything**, write `ANSWER.md`:

1. The line that performs the bad access, and whether it is a read or a write.
2. The line whose call freed the memory, and why that call freed it.
3. The line that originally allocated it.
4. Why the plain build (`make run`) may pass its checks anyway. On the course machine it did, but
   that is not guaranteed. Nothing about this program's behaviour on the plain build is.
5. A fix that keeps the "current best" feature. Describe it; don't paste a whole program.

Then run `make asan` yourself and confirm. Addresses, thread ids and PIDs in your run will
differ from the capture below. The line numbers, function names and the shape of the stacks will not.

## The report

Captured with `make capture TOOL=asan` from the shipped `main.c` (excerpt of `capture.txt`: the
report from the first line to `SUMMARY`. The shadow-byte map that follows is in the file):

```
==782764==ERROR: AddressSanitizer: heap-use-after-free on address 0x6fa83d8e0014 at pc 0x5c9f9a432dc7 bp 0x7fffd535ff50 sp 0x7fffd535ff40
READ of size 4 at 0x6fa83d8e0014 thread T0
    #0 0x5c9f9a432dc6 in main /home/n8/dev/c/LearnCMemoryByGrinding/units/m09-vec/e04-held-pointer/main.c:40
    #1 0x73883ea27780  (/usr/lib/libc.so.6+0x27780) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #2 0x73883ea278b8 in __libc_start_main (/usr/lib/libc.so.6+0x278b8) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #3 0x5c9f9a4321b4 in _start (/home/n8/dev/c/LearnCMemoryByGrinding/units/m09-vec/e04-held-pointer/build/asan/prog+0x31b4) (BuildId: 7103e2226aec4beff0d0be7cf2224af1f6e93787)

0x6fa83d8e0014 is located 4 bytes inside of 16-byte region [0x6fa83d8e0010,0x6fa83d8e0020)
freed by thread T0 here:
    #0 0x73883f72b161  (/usr/lib/libasan.so.8+0x12b161) (BuildId: b8a4241051a1621937fdc46e867ba7ecb56d96ea)
    #1 0x5c9f9a4326a5 in vec_push /home/n8/dev/c/LearnCMemoryByGrinding/units/m09-vec/e04-held-pointer/main.c:17
    #2 0x5c9f9a432b9e in main /home/n8/dev/c/LearnCMemoryByGrinding/units/m09-vec/e04-held-pointer/main.c:35
    #3 0x73883ea27780  (/usr/lib/libc.so.6+0x27780) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #4 0x73883ea278b8 in __libc_start_main (/usr/lib/libc.so.6+0x278b8) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #5 0x5c9f9a4321b4 in _start (/home/n8/dev/c/LearnCMemoryByGrinding/units/m09-vec/e04-held-pointer/build/asan/prog+0x31b4) (BuildId: 7103e2226aec4beff0d0be7cf2224af1f6e93787)

previously allocated by thread T0 here:
    #0 0x73883f72b161  (/usr/lib/libasan.so.8+0x12b161) (BuildId: b8a4241051a1621937fdc46e867ba7ecb56d96ea)
    #1 0x5c9f9a4326a5 in vec_push /home/n8/dev/c/LearnCMemoryByGrinding/units/m09-vec/e04-held-pointer/main.c:17
    #2 0x5c9f9a432b9e in main /home/n8/dev/c/LearnCMemoryByGrinding/units/m09-vec/e04-held-pointer/main.c:35
    #3 0x73883ea27780  (/usr/lib/libc.so.6+0x27780) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #4 0x73883ea278b8 in __libc_start_main (/usr/lib/libc.so.6+0x278b8) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #5 0x5c9f9a4321b4 in _start (/home/n8/dev/c/LearnCMemoryByGrinding/units/m09-vec/e04-held-pointer/build/asan/prog+0x31b4) (BuildId: 7103e2226aec4beff0d0be7cf2224af1f6e93787)

SUMMARY: AddressSanitizer: heap-use-after-free /home/n8/dev/c/LearnCMemoryByGrinding/units/m09-vec/e04-held-pointer/main.c:40 in main
```

## Verify

After applying your fix, `make check` is clean and prints `m09-e04-held-pointer: all checks passed`.
