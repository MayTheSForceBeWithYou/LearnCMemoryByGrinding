TAG = TRACE
SRCS = main.c
include ../../../common.mk

# Batch gdb session used for capture.txt (make capture TOOL=gdb-trace).
gdb-trace: build
	gdb -q -batch -x trace.gdb ./$(PLAIN_BIN) 2>&1
