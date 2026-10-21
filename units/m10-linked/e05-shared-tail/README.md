# m10-e05-shared-tail `[REPORT]`

## Goal

`main.c` builds two lists, "appends" the second to a copy of the first, and destroys
everything at the end. Below is the ASan report from `make asan` on the shipped source.

**From the report and the source alone, before running anything**, write `ANSWER.md`:

1. The kind of error ASan stopped on, whether it was a read or a write, and its line, plus which of the
   three `list_destroy` calls in `main` was running.
2. The line that freed that block, and which `list_destroy` call it was part of.
3. Which node this is (by its name in the program's data), and which function made it reachable from two lists.
4. The report is not a double free, yet the program does contain one. If ASan had not halted, which `free`
   would have been the first to run twice?
5. The ownership rule the program broke, in one sentence, and the fix you'd make. Describe it; don't
   paste a whole program.

Then run `make asan` and confirm. Addresses and thread ids will differ in your run. Lines and
stacks will not.

## The report

Captured with `make capture TOOL=asan` from the shipped `main.c` (excerpt of `capture.txt`: the
report from the first line to `SUMMARY`. The shadow-byte map that follows is in the file):

```
==798349==ERROR: AddressSanitizer: heap-use-after-free on address 0x782beb6e0098 at pc 0x628fb73f94e9 bp 0x7fff99aaff60 sp 0x7fff99aaff50
READ of size 8 at 0x782beb6e0098 thread T0
    #0 0x628fb73f94e8 in list_destroy /home/n8/dev/c/LearnCMemoryByGrinding/units/m10-linked/e05-shared-tail/main.c:17
    #1 0x628fb73fa2d3 in main /home/n8/dev/c/LearnCMemoryByGrinding/units/m10-linked/e05-shared-tail/main.c:83
    #2 0x7c0bec827780  (/usr/lib/libc.so.6+0x27780) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #3 0x7c0bec8278b8 in __libc_start_main (/usr/lib/libc.so.6+0x278b8) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #4 0x628fb73f91a4 in _start (/home/n8/dev/c/LearnCMemoryByGrinding/units/m10-linked/e05-shared-tail/build/asan/prog+0x31a4) (BuildId: b291f4587c068b5870233f0e7985e5bf08c46770)

0x782beb6e0098 is located 8 bytes inside of 16-byte region [0x782beb6e0090,0x782beb6e00a0)
freed by thread T0 here:
    #0 0x7c0bed52ae91  (/usr/lib/libasan.so.8+0x12ae91) (BuildId: b8a4241051a1621937fdc46e867ba7ecb56d96ea)
    #1 0x628fb73f9558 in list_destroy /home/n8/dev/c/LearnCMemoryByGrinding/units/m10-linked/e05-shared-tail/main.c:19
    #2 0x628fb73fa2c4 in main /home/n8/dev/c/LearnCMemoryByGrinding/units/m10-linked/e05-shared-tail/main.c:82
    #3 0x7c0bec827780  (/usr/lib/libc.so.6+0x27780) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #4 0x7c0bec8278b8 in __libc_start_main (/usr/lib/libc.so.6+0x278b8) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #5 0x628fb73f91a4 in _start (/home/n8/dev/c/LearnCMemoryByGrinding/units/m10-linked/e05-shared-tail/build/asan/prog+0x31a4) (BuildId: b291f4587c068b5870233f0e7985e5bf08c46770)

previously allocated by thread T0 here:
    #0 0x7c0bed52c0c1 in malloc (/usr/lib/libasan.so.8+0x12c0c1) (BuildId: b8a4241051a1621937fdc46e867ba7ecb56d96ea)
    #1 0x628fb73f9652 in list_from /home/n8/dev/c/LearnCMemoryByGrinding/units/m10-linked/e05-shared-tail/main.c:28
    #2 0x628fb73fa072 in main /home/n8/dev/c/LearnCMemoryByGrinding/units/m10-linked/e05-shared-tail/main.c:69
    #3 0x7c0bec827780  (/usr/lib/libc.so.6+0x27780) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #4 0x7c0bec8278b8 in __libc_start_main (/usr/lib/libc.so.6+0x278b8) (BuildId: d320029ad1a23be2f4bce117fd321cbd23a2014b)
    #5 0x628fb73f91a4 in _start (/home/n8/dev/c/LearnCMemoryByGrinding/units/m10-linked/e05-shared-tail/build/asan/prog+0x31a4) (BuildId: b291f4587c068b5870233f0e7985e5bf08c46770)

SUMMARY: AddressSanitizer: heap-use-after-free /home/n8/dev/c/LearnCMemoryByGrinding/units/m10-linked/e05-shared-tail/main.c:17 in list_destroy
```

## Verify

After your fix, `make check` is clean and prints `m10-e05-shared-tail: all checks passed`.
