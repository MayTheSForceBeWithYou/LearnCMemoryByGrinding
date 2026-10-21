TAG = PREDICT
SRCS = main.c
include ../../../common.mk

# Memcheck with every leak kind shown, including "still reachable".
valgrind-all: build
	valgrind --leak-check=full --show-leak-kinds=all ./$(PLAIN_BIN)
