TAG = PREDICT
SRCS = main.c
include ../../../common.mk

# Captures quoted in ../LESSON.md:
#   make capture TOOL=demo-read-asan && mv capture.txt capture-read-asan.txt
#   make capture TOOL=demo-read-vg   && mv capture.txt capture-read-vg.txt
#   make capture TOOL=demo-tcache    && mv capture.txt capture-tcache.txt
demo-read-asan: build-asan
	$(ASAN_ENV) ./$(ASAN_BIN) read
demo-read-vg: build
	$(VALGRIND) ./$(PLAIN_BIN) read
demo-tcache: build
	./$(PLAIN_BIN) read; echo "exit $$?"; ./$(PLAIN_BIN) tcache; echo "exit $$?"
