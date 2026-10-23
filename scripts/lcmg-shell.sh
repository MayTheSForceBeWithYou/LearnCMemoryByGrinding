#!/usr/bin/env bash
# lcmg-shell.sh [CMD...] -- open a shell in the course's Linux toolchain container,
# or run CMD inside it. The repo is mounted at /course.
#
#   ./scripts/lcmg-shell.sh                          # interactive shell
#   ./scripts/lcmg-shell.sh make -C units/m01-lifetimes/e01-trace-frames check
#   ./scripts/lcmg-shell.sh bash scripts/verify-all.sh
#
# The course needs Linux/glibc: LeakSanitizer, Valgrind, gdb, GNU ld --wrap,
# reallocarray and mcheck.h do not exist on macOS. See Dockerfile.
set -euo pipefail
root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
image=${LCMG_IMAGE:-lcmg}

if ! docker image inspect "$image" >/dev/null 2>&1; then
    echo "lcmg-shell: building image '$image'..." >&2
    docker build -t "$image" "$root"
fi

# -i only when stdin is a TTY, so this works in scripts and CI too.
tty_flags=(-i)
[ -t 0 ] && tty_flags=(-it)

exec docker run --rm "${tty_flags[@]}" \
    -v "$root":/course \
    -w /course \
    "$image" "${@:-bash}"
