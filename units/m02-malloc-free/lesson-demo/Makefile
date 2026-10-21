TAG = PREDICT NO-ASAN
SRCS = main.c
EXPECT_SYMBOLS =
include ../../../common.mk

# Capture quoted in ../LESSON.md and README.md (plain build only: ASan replaces malloc):
#   make capture TOOL=demo-perturb && mv capture.txt capture-perturb.txt
demo-perturb: build
	./$(PLAIN_BIN); echo "exit $$?"
	MALLOC_PERTURB_=170 ./$(PLAIN_BIN); echo "exit $$?"
	GLIBC_TUNABLES=glibc.malloc.tcache_count=0 MALLOC_PERTURB_=170 ./$(PLAIN_BIN); echo "exit $$?"
