#!/usr/bin/env bash
# fault-sweep.sh BIN [ARGS...] -- run by `make fault` and `make check` ([FAULT]).
# 1. Runs BIN once with FA_COUNT=1 to learn N, the number of allocation calls.
# 2. Runs BIN with FAIL_AT=1..N. Each run must exit 0 (checks pass) or 3 (EXIT_OOM)
#    and print no ASan/LSan/UBSan report.
# Last line: "fault-sweep: N=<n> pass=<p> oom=<o> fail=<f>"; exit 0 iff f == 0.
set -u
bin=$1; shift
export ASAN_OPTIONS=${ASAN_OPTIONS:-detect_leaks=1:abort_on_error=1:halt_on_error=1:allocator_may_return_null=1}
unset FAIL_AT FA_COUNT
log=$(mktemp); trap 'rm -f "$log"' EXIT
report_re='ERROR: (AddressSanitizer|LeakSanitizer)|runtime error:|SUMMARY: (Address|Leak|UndefinedBehavior)Sanitizer'

# Run in the background and wait, so bash does not print "Aborted" job notices.
FA_COUNT=1 "$bin" "$@" >"$log" 2>&1 & wait $!; rc=$?
if [ "$rc" -ne 0 ] || grep -qE "$report_re" "$log"; then
    echo "fault-sweep: baseline run (no injected failure) failed, exit $rc:"; cat "$log"
    echo "fault-sweep: N=? pass=0 oom=0 fail=1"; exit 1
fi
n=$(sed -n 's/^failalloc: \([0-9][0-9]*\) allocations$/\1/p' "$log" | tail -n 1)
if [ -z "$n" ]; then
    echo "fault-sweep: no allocation count printed (is this a fault build?)"
    echo "fault-sweep: N=? pass=0 oom=0 fail=1"; exit 1
fi
[ "$n" -eq 0 ] && echo "fault-sweep: warning: program made no counted allocations"

pass=0 oom=0 fail=0 shown=0
for ((i = 1; i <= n; i++)); do
    FAIL_AT=$i "$bin" "$@" >"$log" 2>&1 & wait $!; rc=$?
    if grep -qE "$report_re" "$log"; then
        verdict="sanitizer report (exit $rc)"
    elif [ "$rc" -eq 0 ]; then pass=$((pass + 1)); continue
    elif [ "$rc" -eq 3 ]; then oom=$((oom + 1)); continue
    else verdict="exit $rc"
    fi
    fail=$((fail + 1))
    echo "fault-sweep: FAIL_AT=$i: $verdict"
    if [ "$shown" -eq 0 ]; then
        shown=1
        grep -E "$report_re|^ *#[0-9]+ |^FAIL " "$log" | head -n 15 | sed 's/^/    /'
    fi
done
echo "fault-sweep: N=$n pass=$pass oom=$oom fail=$fail"
[ "$fail" -eq 0 ]
