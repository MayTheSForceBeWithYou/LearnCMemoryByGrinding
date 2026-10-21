TAG = PREDICT
SRCS = main.c
include ../../../common.mk

# Captures quoted in ../LESSON.md (run `ulimit -s` first; the course machine has 8192):
#   make capture TOOL=demo-250k      && mv capture.txt capture-run-250k.txt
#   make capture TOOL=demo-300k      && mv capture.txt capture-run-300k.txt
#   make capture TOOL=demo-asan-250k && mv capture.txt capture-asan-250k.txt
#   make capture TOOL=demo-asan-300k && mv capture.txt capture-asan-300k.txt
demo-250k: build
	@echo "ulimit -s: $$(ulimit -s)"
	./$(PLAIN_BIN) 250000
demo-300k: build
	@echo "ulimit -s: $$(ulimit -s)"
	./$(PLAIN_BIN) 300000
demo-asan-250k: build-asan
	@echo "ulimit -s: $$(ulimit -s)"
	$(ASAN_ENV) ./$(ASAN_BIN) 250000
demo-asan-300k: build-asan
	@echo "ulimit -s: $$(ulimit -s)"
	$(ASAN_ENV) ./$(ASAN_BIN) 300000
