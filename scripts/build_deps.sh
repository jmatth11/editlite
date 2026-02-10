#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

ZIG_BIN=$1
if [[ -z "$ZIG_BIN" ]]; then
  ZIG_BIN=zig
fi

cd "$SCRIPT_DIR"/../deps/tomlc99
make
cd -

cd "$SCRIPT_DIR"/../deps/utf8-zig
 $ZIG_BIN build -Doptimize=ReleaseSafe
cd -

cd "$SCRIPT_DIR"/../deps/scribe
 $ZIG_BIN build -Doptimize=ReleaseSafe
cd -
