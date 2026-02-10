#!/usr/bin/env bash

zig_version=0.15.2
local_os=$(uname -s)
case "$local_os" in
  "Linux")
    local_os="linux"
    ;;
  "Darwin")
    local_os="macos"
    ;;
  "FreeBSD")
    local_os="freebsd"
    ;;
  "NetBSD")
    local_os="netbsd"
    ;;
  *)
    echo "Platform not supported"
    exit 1
esac

function build_proj() {
  local_arch=$(uname -m)
  mkdir /tmp/zig
  curl -o /tmp/zig/zig.tar.xz  "https://ziglang.org/download/$zig_version/zig-$local_arch-$local_os-$zig_version.tar.xz"
  tar -xf /tmp/zig/zig.tar.xz -C /tmp/zig/
  ./scripts/install_deps.sh "/tmp/zig/zig-$local_arch-$local_os-$zig_version/zig"
  "/tmp/zig/zig-$local_arch-$local_os-$zig_version/zig" build -Doptimize=ReleaseSafe
  rm -rf /tmp/zig
}

which zig > /dev/null 2>&1
if [ $? -ne 0 ]; then
  echo "zig not found. pulling temporary zig for build."
  build_proj
  exit 0
fi

local_version=$(zig version)
if [ "$zig_version" != "$local_version" ]; then
  echo "zig version doesn't match. pulling temporary zig for build."
  build_proj
  exit 0
fi

./scripts/install_deps.sh
zig build -Doptimize=ReleaseSafe
