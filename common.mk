# common.mk -- shared build rules for every LearnCMemoryByGrinding exercise.
#
# An exercise Makefile sets a few variables and includes this file:
#
#     TAG = FAULT
#     SRCS = main.c
#     EXPECT_SYMBOLS = record_create record_destroy
#     include ../../../common.mk
#
# Variables an exercise may set (all optional except TAG):
#   TAG             WRITE BUG REPORT PREDICT TRACE FAULT DESIGN; add NO-ASAN to skip ASan/fault
#   SRCS            C sources of the program (default: main.c)
#   EXPECT_SYMBOLS  functions the student must define (WRITE/FAULT/DESIGN starters)
#   ARGS            command-line arguments passed to the program
#   EXTRA_CFLAGS    extra compile flags (e.g. -D_DEFAULT_SOURCE)
#   LDLIBS          extra libraries (e.g. -lm)
#   ASAN_EXTRA      extra ASAN_OPTIONS appended for asan/fault (e.g. detect_stack_use_after_return=1)
#
# Targets: make / make run, make asan, make valgrind, make fault, make leaks,
#          make check, make clean, make capture TOOL=asan|valgrind|run (REPORT exercises).
#
# Platforms: Linux (gcc or clang) and macOS on Apple Silicon (Apple Clang).
# On macOS, `gcc` is Apple's clang (see docs/macos.md). Valgrind does not exist
# there: `make check` runs the `leaks` tool instead, and `make valgrind` explains
# itself and exits 1. LeakSanitizer is also unsupported on macOS, so ASan runs
# with detect_leaks=0 and leak checking is the `leaks` step.

# Must be evaluated before anything else is included: the directory of this file.
COURSE_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
LIBDIR      := $(COURSE_ROOT)/lib

# Platform detection. UNAME_S is overridable on the command line, which is how
# the macOS build logic is exercised on Linux (make UNAME_S=Darwin ...).
UNAME_S   := $(shell uname -s)
IS_DARWIN := $(if $(filter Darwin,$(UNAME_S)),yes)

TAG            ?= WRITE
SRCS           ?= main.c
EXPECT_SYMBOLS ?=
ARGS           ?=
PROG           ?= prog
# gcc by default; `make CC=clang` or CC=clang in the environment wins.
# Not `CC ?= gcc`: make predefines CC as `cc`, so ?= would never fire and would
# silently compile with cc instead. $(origin) is `default` only for make's own value.
ifeq ($(filter-out default undefined,$(origin CC)),)
CC             := gcc
endif

CFLAGS  := -std=c17 -Wall -Wextra -Wpedantic -g -I$(LIBDIR) $(EXTRA_CFLAGS)
# Strict -std=c17 hides the POSIX declarations (strdup, strndup, getline) from
# the macOS SDK headers. -D_DARWIN_C_SOURCE is the macOS equivalent of glibc's
# _DEFAULT_SOURCE: it restores the full API without dropping the standard mode.
# -Werror=implicit-function-declaration matches Apple Clang's default (it
# errors where GCC only warns) so the Linux Darwin simulation catches
# undeclared calls the same way the macOS runner does.
ifeq ($(IS_DARWIN),yes)
CFLAGS  += -D_DARWIN_C_SOURCE -Werror=implicit-function-declaration
endif
# reallocarray is absent from the macOS SDK entirely; the student's calls are
# renamed to lcmg_reallocarray (lib/lcmg_reallocarray.c, linked below). This
# rename lives in its own variable, applied to the plain and asan objects
# only: the fault objects get their own rename from FAULT_DEFS, and defining
# the macro twice on one command line warns (-Wmacro-redefined).
ifeq ($(IS_DARWIN),yes)
REALLOC_DEF := -Dreallocarray=lcmg_reallocarray
endif
SANFLAGS := -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer
# GNU ld --wrap (Linux) rewrites the student's malloc calls to __wrap_malloc at
# link time. Apple's ld64 rejects --wrap, so on macOS the same renaming happens
# at compile time for the fault objects only (see FAULT_DEFS below), and
# -fno-builtin stops the compiler folding the renamed calls back into builtins.
WRAPFLAGS := -Wl,--wrap=malloc,--wrap=calloc,--wrap=realloc,--wrap=reallocarray,--wrap=free,--wrap=strdup,--wrap=strndup
FAULT_DEFS := -Dmalloc=__wrap_malloc -Dcalloc=__wrap_calloc -Drealloc=__wrap_realloc \
              -Dreallocarray=__wrap_reallocarray -Dfree=__wrap_free \
              -Dstrdup=__wrap_strdup -Dstrndup=__wrap_strndup
ifeq ($(IS_DARWIN),yes)
WRAPFLAGS :=
endif

ASAN_EXTRA ?=
# LeakSanitizer is unsupported on macOS: detect_leaks=1 makes ASan abort at
# startup, before main runs. Leak checking on macOS is the `leaks` target.
ifeq ($(IS_DARWIN),yes)
ASAN_ENV := ASAN_OPTIONS=detect_leaks=0:abort_on_error=1:halt_on_error=1:allocator_may_return_null=1$(if $(ASAN_EXTRA),:$(ASAN_EXTRA)) UBSAN_OPTIONS=print_stacktrace=1
else
ASAN_ENV := ASAN_OPTIONS=detect_leaks=1:abort_on_error=1:halt_on_error=1:allocator_may_return_null=1$(if $(ASAN_EXTRA),:$(ASAN_EXTRA)) UBSAN_OPTIONS=print_stacktrace=1
endif
VALGRIND := valgrind --leak-check=full --show-leak-kinds=definite,indirect,possible \
            --errors-for-leak-kinds=definite,indirect,possible --error-exitcode=1 --track-origins=yes
LEAKS := leaks --atExit --

# Arch ships a stripped ld.so; Valgrind needs debuginfod to start (appendix C of the RPG course).
export DEBUGINFOD_URLS ?= https://debuginfod.archlinux.org

B          := build
PLAIN_BIN  := $(B)/plain/$(PROG)
ASAN_BIN   := $(B)/asan/$(PROG)
FAULT_BIN  := $(B)/fault/$(PROG)
PLAIN_OBJS := $(patsubst %.c,$(B)/plain/%.o,$(SRCS))
ASAN_OBJS  := $(patsubst %.c,$(B)/asan/%.o,$(SRCS))
FAULT_OBJS := $(patsubst %.c,$(B)/fault/%.o,$(SRCS)) $(B)/fault/_failalloc.o
# macOS only: the course-provided reallocarray (lib/lcmg_reallocarray.c),
# linked into every binary. Compiled plain, like failalloc.c, so its realloc
# call always reaches the real allocator.
ifeq ($(IS_DARWIN),yes)
PLAIN_OBJS += $(B)/plain/_reallocarray.o
ASAN_OBJS  += $(B)/asan/_reallocarray.o
FAULT_OBJS += $(B)/fault/_reallocarray.o
endif
HDRS       := $(wildcard *.h) $(wildcard $(LIBDIR)/*.h)

USE_ASAN  := $(if $(filter NO-ASAN,$(TAG)),,yes)
USE_FAULT := $(if $(USE_ASAN),$(filter FAULT,$(TAG)))

.DEFAULT_GOAL := run
.PHONY: run build build-asan build-fault build-all asan valgrind leaks fault check clean capture print-src-hash
.DELETE_ON_ERROR:

$(B)/plain/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(REALLOC_DEF) -c $< -o $@
$(B)/asan/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SANFLAGS) $(REALLOC_DEF) -c $< -o $@
# Fault objects: the student's own sources get the allocation renames (macOS) and
# the --wrap link flags (Linux). failalloc.c itself is compiled plain, below, so
# its calls to the real allocator are never renamed.
$(B)/fault/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SANFLAGS) $(if $(IS_DARWIN),$(FAULT_DEFS) -fno-builtin) -c $< -o $@
$(B)/fault/_failalloc.o: $(LIBDIR)/failalloc.c $(LIBDIR)/failalloc.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SANFLAGS) -c $< -o $@
$(B)/plain/_reallocarray.o: $(LIBDIR)/lcmg_reallocarray.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
$(B)/asan/_reallocarray.o: $(LIBDIR)/lcmg_reallocarray.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SANFLAGS) -c $< -o $@
$(B)/fault/_reallocarray.o: $(LIBDIR)/lcmg_reallocarray.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SANFLAGS) -c $< -o $@

$(PLAIN_BIN): $(PLAIN_OBJS)
	$(CC) $^ -o $@ $(LDLIBS)
$(ASAN_BIN): $(ASAN_OBJS)
	$(CC) $(SANFLAGS) $^ -o $@ $(LDLIBS)
$(FAULT_BIN): $(FAULT_OBJS)
	$(CC) $(SANFLAGS) $(WRAPFLAGS) $^ -o $@ $(LDLIBS)

build: $(PLAIN_BIN)
build-asan: $(ASAN_BIN)
build-fault: $(FAULT_BIN)
build-all: build $(if $(USE_ASAN),build-asan) $(if $(USE_FAULT),build-fault)

run: $(PLAIN_BIN)
	./$(PLAIN_BIN) $(ARGS)

# abort_on_error=1 makes ASan die by SIGABRT; the shell reports 134 and make fails.
asan: $(ASAN_BIN)
	$(ASAN_ENV) ./$(ASAN_BIN) $(ARGS)

valgrind: $(PLAIN_BIN)
ifeq ($(IS_DARWIN),yes)
	@echo "valgrind: not available on macOS (there is no Valgrind port for Apple Silicon)." >&2
	@echo "Use 'make asan' plus 'make leaks' instead. Full Valgrind runs in the Linux" >&2
	@echo "container: ./scripts/lcmg-shell.sh make -C $(CURDIR) valgrind" >&2
	@exit 1
else
	$(VALGRIND) ./$(PLAIN_BIN) $(ARGS)
endif

# macOS built-in leak checker: runs the program and reports leaked blocks at exit.
# Exits non-zero when leaks are found, so `make check` can rely on it.
leaks: $(PLAIN_BIN)
ifeq ($(IS_DARWIN),yes)
	$(LEAKS) ./$(PLAIN_BIN) $(ARGS)
else
	@echo "leaks: macOS only (on Linux use 'make valgrind')" >&2
	@exit 1
endif

fault: $(FAULT_BIN)
	@$(ASAN_ENV) bash $(COURSE_ROOT)/scripts/fault-sweep.sh ./$(FAULT_BIN) $(ARGS)

check: build-all
	@echo "== checks (plain)"
	@./$(PLAIN_BIN) $(ARGS)
ifneq ($(USE_ASAN),)
	@echo "== asan"
	@$(ASAN_ENV) ./$(ASAN_BIN) $(ARGS)
endif
ifeq ($(IS_DARWIN),yes)
	@echo "== leaks (macOS leaks tool; Valgrind is unavailable here)"
	@$(LEAKS) ./$(PLAIN_BIN) $(ARGS)
else
	@echo "== valgrind"
	@$(VALGRIND) -q ./$(PLAIN_BIN) $(ARGS)
endif
ifneq ($(USE_FAULT),)
	@echo "== fault sweep"
	@$(ASAN_ENV) bash $(COURSE_ROOT)/scripts/fault-sweep.sh ./$(FAULT_BIN) $(ARGS)
endif
	@echo "== check: OK"

clean:
	rm -rf $(B)

# --- REPORT exercises -------------------------------------------------------
# sha256sum is GNU coreutils; macOS ships shasum -a 256 instead.
SHA256SUM := $(if $(shell command -v sha256sum),sha256sum,shasum -a 256)
SRC_HASH = $(shell cat $(sort $(SRCS) $(wildcard *.h)) | $(SHA256SUM) | cut -d' ' -f1)
TOOL ?= asan
print-src-hash:
	@echo $(SRC_HASH)
capture: build-all
	@{ echo "# source-sha256: $(SRC_HASH)"; \
	   echo "# tool: $(TOOL)   captured: $$(date '+%Y-%m-%d %H:%M %Z') on $$(uname -n)"; \
	   echo "# Addresses, PIDs and thread ids will differ on your run."; \
	   $(MAKE) -s --no-print-directory $(TOOL) 2>&1; echo "# exit status: $$?"; } > capture.txt; \
	 echo "wrote capture.txt"
