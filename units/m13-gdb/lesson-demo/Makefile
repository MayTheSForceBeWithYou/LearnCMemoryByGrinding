TAG = PREDICT
SRCS = main.c
include ../../../common.mk

GDB := gdb -q -batch -iex 'set debuginfod enabled off'
# Captures quoted in ../LESSON.md:
#   make capture TOOL=demo-grow  && mv capture.txt capture-grow.txt
#   make capture TOOL=demo-watch && mv capture.txt capture-watch.txt
#   make capture TOOL=demo-asan  && mv capture.txt capture-asan.txt
#   make capture TOOL=demo-vgdb  && mv capture.txt capture-vgdb.txt
demo-grow: build
	$(GDB) -x grow.gdb ./$(PLAIN_BIN) 2>&1
demo-watch: build
	$(GDB) -x watch.gdb ./$(PLAIN_BIN) 2>&1
demo-asan: build-asan
	$(GDB) -x asan.gdb ./$(ASAN_BIN) 2>&1
demo-vgdb: build
	@rm -f $(B)/vgdb-*
	@valgrind --vgdb=yes --vgdb-error=0 --vgdb-prefix=$(B)/vgdb ./$(PLAIN_BIN) uninit 2>$(B)/vg.log & \
	  sleep 2; \
	  $(GDB) -ex 'target remote | vgdb --vgdb-prefix=$(B)/vgdb' -ex continue -ex 'bt' \
	    -ex 'print v[1]' -ex 'eval "monitor get_vbits %p 8", v' -ex 'kill' ./$(PLAIN_BIN) 2>&1; \
	  wait; echo '--- valgrind stderr ---'; cat $(B)/vg.log
