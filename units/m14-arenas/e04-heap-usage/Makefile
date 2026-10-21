TAG = PREDICT
SRCS = main.c
include ../../../common.mk

# Valgrind's heap summary for both strategies.
usage: build
	@for m in list arena; do \
	  echo "### $$m"; valgrind ./$(PLAIN_BIN) $$m 2>&1 | grep -E 'sum =|total heap usage|in use at exit'; \
	done
