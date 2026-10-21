set debuginfod enabled off
set pagination off
set breakpoint pending on
set environment ASAN_OPTIONS=detect_leaks=1:abort_on_error=1:halt_on_error=1
break main
run asan
break __asan_report_error
rbreak ^__asan_report_store[0-9]+$
break __asan_report_store4
info breakpoints
continue
bt 3
continue
bt 12
