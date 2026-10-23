# Linux toolchain for LearnCMemoryByGrinding.
# The course targets Linux/glibc: LeakSanitizer, Valgrind, gdb, GNU ld --wrap,
# glibc reallocarray and mcheck.h are all unavailable on macOS. Build and work here.
#
#   docker build -t lcmg .
#   ./scripts/lcmg-shell.sh
FROM debian:trixie-slim

ENV DEBIAN_FRONTEND=noninteractive

# The slim image excludes /usr/share/man via dpkg path-exclude; the course needs man.
RUN rm -f /etc/dpkg/dpkg.cfg.d/docker-apt-speedup /etc/dpkg/dpkg.cfg.d/docker \
 && apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        clang \
        libclang-rt-dev \
        gdb \
        valgrind \
        libc6-dbg \
        make \
        bash \
        coreutils \
        manpages \
        manpages-dev \
        man-db \
        less \
        ca-certificates \
 && rm -rf /var/lib/apt/lists/*

# The course README's DEBUGINFOD workaround is Arch-specific; Debian ships an
# unstripped ld.so, so Valgrind starts without it. Leave debuginfod off.
ENV DEBUGINFOD_URLS=

WORKDIR /course
CMD ["/bin/bash"]
