TAG = PREDICT
SRCS = main.c
include ../../../common.mk

# Runs every scenario under the plain build, ASan and Valgrind, and reports exit status.
matrix: build build-asan
	@for s in nul uninit stack; do \
	  echo "### $$s: plain"; ./$(PLAIN_BIN) $$s; echo "exit $$?"; \
	  echo "### $$s: asan"; $(ASAN_ENV) ./$(ASAN_BIN) $$s 2>&1 | grep -E '^(==[0-9]+==ERROR|SUMMARY|[a-z].*runtime error)'; echo "exit $${PIPESTATUS[0]}"; \
	  echo "### $$s: valgrind"; $(VALGRIND) -q ./$(PLAIN_BIN) $$s 2>&1 | grep -E '^==[0-9]+== [A-Z]'; echo "exit $${PIPESTATUS[0]}"; \
	done
