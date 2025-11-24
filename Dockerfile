# build step
FROM ubuntu:latest AS builder

RUN apt update && \
    apt upgrade

RUN apt-get install -y gcc \
      clang \
      cmake \
      make \
      git \
      wget \
      libsdl2-dev \
      libsdl2-ttf-dev \
      xz-utils \
      libicu-dev

RUN mkdir -p /tmp/libicu
RUN wget -O /tmp/libicu/icu_amd64.deb http://archive.ubuntu.com/ubuntu/pool/main/i/icu/libicu70_70.1-2_amd64.deb
RUN dpkg -i /tmp/libicu/icu_amd64.deb

WORKDIR /app
RUN mkdir -p /tmp/zig
RUN wget -O /tmp/zig/zig.tar.xz "https://ziglang.org/download/0.15.2/zig-x86_64-linux-0.15.2.tar.xz"
RUN tar -xf /tmp/zig/zig.tar.xz -C /tmp/zig/

ENV PATH="${PATH}:/tmp/zig/zig-x86_64-linux-0.15.2/"

ENTRYPOINT ["make"]

