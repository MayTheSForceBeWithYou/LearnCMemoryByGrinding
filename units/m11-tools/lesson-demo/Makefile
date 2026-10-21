TAG = PREDICT
SRCS = main.c
include ../../../common.mk

# Captures quoted in ../LESSON.md:
#   make capture TOOL=demo-asan     && mv capture.txt capture-asan.txt
#   make capture TOOL=demo-lsan     && mv capture.txt capture-lsan.txt
#   make capture TOOL=demo-vg-leaks && mv capture.txt capture-vg-leaks.txt
#   make capture TOOL=demo-origins  && mv capture.txt capture-origins.txt
#   make capture TOOL=demo-mtrace   && mv capture.txt capture-mtrace.txt
demo-asan: build-asan
	$(ASAN_ENV) ./$(ASAN_BIN) overflow
demo-lsan: build-asan
	$(ASAN_ENV) ./$(ASAN_BIN) leaks
demo-vg-leaks: build
	valgrind --leak-check=full --show-leak-kinds=all ./$(PLAIN_BIN) leaks
demo-origins: build
	valgrind --track-origins=yes ./$(PLAIN_BIN) uninit
demo-mtrace: build
	MALLOC_TRACE=$(B)/mtrace.log LD_PRELOAD=/usr/lib/libc_malloc_debug.so.0 ./$(PLAIN_BIN) leaks
	mtrace ./$(PLAIN_BIN) $(B)/mtrace.log
