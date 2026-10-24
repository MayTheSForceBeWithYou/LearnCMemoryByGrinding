#!/usr/bin/env bash
# verify-all.sh [--only PATH]... [COURSE_DIR] -- verify exercises and solutions.
#
# COURSE_DIR defaults to $LCMG_ROOT, else the directory above this script.
# --only PATH (repeatable) limits the run to exercises whose path relative to
# the course root starts with PATH, e.g. --only units/m12-zoo --only capstones/c3-lru.
# Their mirrored solutions are checked with them.
# Walks units/*/e*/ and capstones/*/ ; the solution of units/U/E is solutions/U/E,
# the solution of capstones/C is solutions/capstones/C.
#   solution (any tag, if it has a Makefile)  make check passes
#   WRITE / FAULT / DESIGN starter            build fails with `undefined reference`
#                                             to one of EXPECT_SYMBOLS, no compile error
#   BUG starter                               builds, then make check fails
#   REPORT starter                            builds, capture.txt source-sha256 matches
#   other tags (PREDICT, TRACE, ...)          builds
#   capture*.txt in a solution dir            source-sha256 matches the solution's own
#                                             source, or the starter's if it has no Makefile
#   units/*/lesson-demo/                      builds; every capture*.txt matches its source
# Every exercise's build/ is removed afterwards. Exit status is non-zero on any failure.
set -u
here=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

# Preconditions. Without these the run used to abort partway and still exit 0,
# reporting success while verifying almost nothing. Fail loudly instead.
# macOS: /bin/bash is 3.2 and there is no GNU `timeout`. Install both from
# Homebrew (brew install bash coreutils) and run with the Homebrew bash:
#   /opt/homebrew/bin/bash scripts/verify-all.sh
# coreutils provides `timeout` as `gtimeout`; either name works here.
if [ "${BASH_VERSINFO[0]}" -lt 4 ]; then
    echo "verify-all: needs bash 4+ (associative arrays); this is bash $BASH_VERSION" >&2
    echo "verify-all: on macOS install it (brew install bash) and run with /opt/homebrew/bin/bash;" >&2
    echo "verify-all: otherwise run it in the course container: ./scripts/lcmg-shell.sh bash scripts/verify-all.sh" >&2
    exit 2
fi
TIMEOUT_BIN=$(command -v timeout || command -v gtimeout || true)
if [ -z "$TIMEOUT_BIN" ]; then
    echo "verify-all: needs 'timeout' (GNU coreutils) to bound each make check" >&2
    echo "verify-all: on macOS install it (brew install coreutils); otherwise run in the" >&2
    echo "verify-all: course container: ./scripts/lcmg-shell.sh bash scripts/verify-all.sh" >&2
    exit 2
fi
# macOS note: `make check` there runs the `leaks` tool instead of Valgrind, so a
# [BUG] starter whose bug is only visible to Memcheck can pass `make check` on a
# Mac. Such starters are reported SKIP (not FAIL) on Darwin; verify them in the
# container for the full verdict.
IS_DARWIN=0
[ "$(uname -s)" = Darwin ] && IS_DARWIN=1
only=()
while [ $# -gt 0 ]; do
    case "$1" in
    --only) only+=("${2%/}"); shift 2 ;;
    --only=*) v=${1#--only=}; only+=("${v%/}"); shift ;;
    *) break ;;
    esac
done
root=$(cd "${1:-${LCMG_ROOT:-$here/..}}" && pwd) || exit 2
wanted() { # wanted REL -> 0 if no --only given or REL matches one
    [ ${#only[@]} -eq 0 ] && return 0
    local o; for o in "${only[@]}"; do case "$1" in "$o"|"$o"/*) return 0 ;; esac; done
    return 1
}
unset FAIL_AT FA_COUNT
log=$(mktemp); trap 'rm -f "$log"' EXIT
MAKE=(make --no-print-directory)
fails=0 total=0

mkvar() { # mkvar DIR NAME -> value of NAME = ... in DIR/Makefile
    sed -n -E "s/^[[:space:]]*$2[[:space:]]*[:?+]?=[[:space:]]*//p" "$1/Makefile" | head -n 1 | sed 's/[[:space:]]*$//'
}
row() { printf '%-46s %-8s %-9s %-5s %s\n' "$1" "$2" "$3" "$4" "$5"; }
result() { # result PATH TAG KIND OK DETAIL
    total=$((total + 1))
    if [ "$4" = 1 ]; then row "$1" "$2" "$3" PASS "$5"
    else row "$1" "$2" "$3" FAIL "$5"; fails=$((fails + 1)); fi
}
# check_captures DIR HASH -> prints "" if every DIR/capture*.txt matches HASH, else the stale names
check_captures() {
    local f want bad=
    for f in "$1"/capture*.txt; do
        [ -f "$f" ] || continue
        want=$(sed -n 's/^# source-sha256: *\([0-9a-f]*\).*/\1/p' "$f" | head -n 1)
        [ "$want" = "$2" ] || bad="$bad ${f##*/}"
    done
    printf '%s' "$bad"
}
src_hash() { "${MAKE[@]}" -s -C "$1" print-src-hash 2>/dev/null; }
clean() { "${MAKE[@]}" -C "$1" clean >/dev/null 2>&1; }
compile_error() { grep -qE '\.[ch]:[0-9]+(:[0-9]+)?: (fatal )?error:' "$log"; }

verify_solution() { # DIR REL
    local d=$1 rel=$2 tag detail ok=0
    tag=$(mkvar "$d" TAG)
    if "$TIMEOUT_BIN" 600 "${MAKE[@]}" -C "$d" check >"$log" 2>&1; then
        ok=1; detail="check ok"
    else
        local why; why=$(grep -m1 -E '^fault-sweep: (FAIL_AT|baseline)' "$log")
        [ -n "$why" ] || why=$(grep -m1 -E '^FAIL |ERROR:|SUMMARY|error:|definitely lost' "$log")
        detail="make check failed at '$(grep '^== ' "$log" | tail -n 1 | sed 's/^== //')': $(echo "$why" | cut -c1-70)"
    fi
    local sweep; sweep=$(sed -n 's/^fault-sweep: N=\(.*\)$/N=\1/p' "$log" | tail -n 1)
    [ -n "$sweep" ] && detail="$detail; sweep $sweep"
    local stale; stale=$(check_captures "$d" "$(src_hash "$d")")
    if [ -n "$stale" ]; then ok=0; detail="$detail; STALE:$stale"; fi
    result "$rel" "$tag" solution $ok "$detail"
    [ $ok = 1 ] || sed 's/^/    | /' "$log" | tail -n 25
    clean "$d"
}

verify_starter() { # DIR REL
    local d=$1 rel=$2 tag kind ok=0 detail
    tag=$(mkvar "$d" TAG)
    kind=$(printf '%s\n' $tag | grep -vx NO-ASAN | head -n 1)
    case "$kind" in
    WRITE|FAULT|DESIGN)
        local syms; syms=$(mkvar "$d" EXPECT_SYMBOLS)
        if [ -z "$syms" ]; then detail="no EXPECT_SYMBOLS in Makefile"
        elif "${MAKE[@]}" -C "$d" build >"$log" 2>&1; then detail="starter builds, but must not"
        elif compile_error; then detail="compile error (not a link failure): $(grep -m1 -E 'error:' "$log" | cut -c1-80)"
        else
            local s hit=
            # GNU ld: "undefined reference to `sym'". Apple ld64 (clang):
            # "Undefined symbols for architecture arm64: \"_sym\", referenced from: ...".
            for s in $syms; do
                grep -q "undefined reference to \`$s'" "$log" && hit="$hit $s"
                grep -q "\"_$s\"" "$log" && hit="$hit $s"
            done
            if [ -n "$hit" ]; then ok=1; detail="undefined reference:$hit"
            else detail="build failed without undefined reference to: $syms"; fi
        fi ;;
    BUG)
        if ! "${MAKE[@]}" -C "$d" build-all >"$log" 2>&1; then detail="does not build"
        elif "$TIMEOUT_BIN" 600 "${MAKE[@]}" -C "$d" check >"$log" 2>&1; then
            if [ "$IS_DARWIN" = 1 ]; then
                # On macOS there is no Valgrind in `make check`; a bug Memcheck
                # alone would catch passes here. Not a failure — just unverified
                # on this platform. See the note at the top of this script.
                result "$rel" "${tag:-?}" starter 1 "SKIP (Darwin): check passes without Valgrind; verify in container"
                clean "$d"; return 0
            fi
            detail="make check passes; bug not detected"
        else ok=1; detail="builds; check fails at: $(grep '^== ' "$log" | tail -n 1 | sed 's/^== //')"; fi ;;
    REPORT)
        if ! "${MAKE[@]}" -C "$d" build-all >"$log" 2>&1; then detail="does not build"
        elif [ ! -f "$d/capture.txt" ]; then detail="no capture.txt (run: make capture TOOL=asan|valgrind)"
        else
            local want have
            want=$(sed -n 's/^# source-sha256: *\([0-9a-f]*\).*/\1/p' "$d/capture.txt" | head -n 1)
            have=$("${MAKE[@]}" -s -C "$d" print-src-hash)
            if [ "$want" = "$have" ]; then ok=1; detail="builds; capture matches source"
            else detail="STALE capture: source changed since capture (re-run make capture)"; fi
        fi ;;
    *)
        if "${MAKE[@]}" -C "$d" build-all >"$log" 2>&1; then ok=1; detail="builds"
        else detail="does not build"; fi ;;
    esac
    result "$rel" "${tag:-?}" starter $ok "$detail"
    [ $ok = 1 ] || sed 's/^/    | /' "$log" | tail -n 15
    clean "$d"
}

row EXERCISE TAG KIND RES DETAIL
row ---------------------------------------------- -------- --------- ----- ------
shopt -s nullglob
declare -A seen
for d in "$root"/units/*/e*/ "$root"/capstones/*/; do
    d=${d%/}; rel=${d#"$root"/}
    wanted "$rel" || continue
    [ -f "$d/Makefile" ] || { result "$rel" ? starter 0 "no Makefile"; continue; }
    verify_starter "$d" "$rel"
    case "$rel" in units/*) srel=solutions/${rel#units/} ;; *) srel=solutions/$rel ;; esac
    seen[$srel]=1
    tag=$(mkvar "$d" TAG)
    if [ -f "$root/$srel/Makefile" ]; then verify_solution "$root/$srel" "$srel"
    elif compgen -G "$root/$srel/capture*.txt" >/dev/null; then
        stale=$(check_captures "$root/$srel" "$(src_hash "$d")")
        if [ -z "$stale" ]; then result "$srel" "$tag" solution 1 "capture matches starter source"
        else result "$srel" "$tag" solution 0 "STALE:$stale (re-capture from the starter)"; fi
    elif printf '%s\n' $tag | grep -qxE 'WRITE|FAULT|DESIGN|BUG'; then
        result "$srel" "$tag" solution 0 "missing solution (Makefile)"
    fi
done
for d in "$root"/units/*/lesson-demo/; do
    d=${d%/}; rel=${d#"$root"/}
    wanted "$rel" || continue
    if ! "${MAKE[@]}" -C "$d" build-all >"$log" 2>&1; then result "$rel" demo demo 0 "does not build"
    else
        stale=$(check_captures "$d" "$(src_hash "$d")")
        if [ -z "$stale" ]; then result "$rel" demo demo 1 "builds; captures match source"
        else result "$rel" demo demo 0 "STALE:$stale"; fi
    fi
    clean "$d"
done
for d in "$root"/solutions/*/e*/ "$root"/solutions/capstones/*/; do
    d=${d%/}; rel=${d#"$root"/}
    case "$rel" in solutions/capstones/*) wanted "${rel#solutions/}" ;; *) wanted "units/${rel#solutions/}" ;; esac || continue
    [ -n "${seen[$rel]:-}" ] || result "$rel" ? solution 0 "orphan: no matching exercise"
done

echo
if [ "$total" -eq 0 ]; then echo "verify-all: no exercises found under $root"; exit 1; fi
echo "verify-all: $((total - fails))/$total passed, $fails failed  ($root)"
[ "$fails" -eq 0 ]
