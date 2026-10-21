TAG = PREDICT
SRCS = main.c
include ../../../common.mk

# Captures quoted in ../LESSON.md:
#   make capture TOOL=run       && mv capture.txt capture-run.txt
#   make capture TOOL=demo-asan && mv capture.txt capture-asan.txt
demo-asan: build-asan
	$(ASAN_ENV) ./$(ASAN_BIN)
