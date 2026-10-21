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
# Targets: make / make run, make asan, make valgrind, make fault, make check,
#          make clean, make capture TOOL=asan|valgrind|run (REPORT exercises).

# Must be evaluated before anything else is included: the directory of this file.
COURSE_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
LIBDIR      := $(COURSE_ROOT)/lib

TAG            ?= WRITE
SRCS           ?= main.c
EXPECT_SYMBOLS ?=
ARGS           ?=
PROG           ?= prog
CC             := gcc

CFLAGS  := -std=c17 -Wall -Wextra -Wpedantic -g -I$(LIBDIR) $(EXTRA_CFLAGS)
SANFLAGS := -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer
WRAPFLAGS := -Wl,--wrap=malloc,--wrap=calloc,--wrap=realloc,--wrap=reallocarray,--wrap=free,--wrap=strdup,--wrap=strndup

ASAN_EXTRA ?=
ASAN_ENV := ASAN_OPTIONS=detect_leaks=1:abort_on_error=1:halt_on_error=1:allocator_may_return_null=1$(if $(ASAN_EXTRA),:$(ASAN_EXTRA)) UBSAN_OPTIONS=print_stacktrace=1
VALGRIND := valgrind --leak-check=full --show-leak-kinds=definite,indirect,possible \
            --errors-for-leak-kinds=definite,indirect,possible --error-exitcode=1 --track-origins=yes

# Arch ships a stripped ld.so; Valgrind needs debuginfod to start (appendix C of the RPG course).
export DEBUGINFOD_URLS ?= https://debuginfod.archlinux.org

B          := build
PLAIN_BIN  := $(B)/plain/$(PROG)
ASAN_BIN   := $(B)/asan/$(PROG)
FAULT_BIN  := $(B)/fault/$(PROG)
PLAIN_OBJS := $(patsubst %.c,$(B)/plain/%.o,$(SRCS))
ASAN_OBJS  := $(patsubst %.c,$(B)/asan/%.o,$(SRCS))
FAULT_OBJS := $(patsubst %.c,$(B)/fault/%.o,$(SRCS)) $(B)/fault/_failalloc.o
HDRS       := $(wildcard *.h) $(wildcard $(LIBDIR)/*.h)

USE_ASAN  := $(if $(filter NO-ASAN,$(TAG)),,yes)
USE_FAULT := $(if $(USE_ASAN),$(filter FAULT,$(TAG)))

.DEFAULT_GOAL := run
.PHONY: run build build-asan build-fault build-all asan valgrind fault check clean capture print-src-hash
.DELETE_ON_ERROR:

$(B)/plain/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
$(B)/asan/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SANFLAGS) -c $< -o $@
$(B)/fault/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SANFLAGS) -c $< -o $@
$(B)/fault/_failalloc.o: $(LIBDIR)/failalloc.c $(LIBDIR)/failalloc.h
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
	$(VALGRIND) ./$(PLAIN_BIN) $(ARGS)

fault: $(FAULT_BIN)
	@$(ASAN_ENV) bash $(COURSE_ROOT)/scripts/fault-sweep.sh ./$(FAULT_BIN) $(ARGS)

check: build-all
	@echo "== checks (plain)"
	@./$(PLAIN_BIN) $(ARGS)
ifneq ($(USE_ASAN),)
	@echo "== asan"
	@$(ASAN_ENV) ./$(ASAN_BIN) $(ARGS)
endif
	@echo "== valgrind"
	@$(VALGRIND) -q ./$(PLAIN_BIN) $(ARGS)
ifneq ($(USE_FAULT),)
	@echo "== fault sweep"
	@$(ASAN_ENV) bash $(COURSE_ROOT)/scripts/fault-sweep.sh ./$(FAULT_BIN) $(ARGS)
endif
	@echo "== check: OK"

clean:
	rm -rf $(B)

# --- REPORT exercises -------------------------------------------------------
SRC_HASH = $(shell cat $(sort $(SRCS) $(wildcard *.h)) | sha256sum | cut -d' ' -f1)
TOOL ?= asan
print-src-hash:
	@echo $(SRC_HASH)
capture: build-all
	@{ echo "# source-sha256: $(SRC_HASH)"; \
	   echo "# tool: $(TOOL)   captured: $$(date '+%Y-%m-%d %H:%M %Z') on $$(uname -n)"; \
	   echo "# Addresses, PIDs and thread ids will differ on your run."; \
	   $(MAKE) -s --no-print-directory $(TOOL) 2>&1; echo "# exit status: $$?"; } > capture.txt; \
	 echo "wrote capture.txt"
