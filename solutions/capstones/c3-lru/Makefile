TAG = FAULT
SRCS = main.c lru.c
EXPECT_SYMBOLS = lru_resize lru_keys
include ../../../common.mk

# Part 1 only: the provided cache, without the Part 2 functions and checks.
part1:
	@$(MAKE) --no-print-directory B=build/part1 EXTRA_CFLAGS=-DLRU_PART1 check
