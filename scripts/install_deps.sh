# install core dependencies.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

if [ ! -d "$SCRIPT_DIR"/../deps/array_template ]; then
  git clone https://github.com/jmatth11/array_template.git "$SCRIPT_DIR"/../deps/array_template
fi
if [ ! -d "$SCRIPT_DIR"/../deps/tmolc99 ]; then
  git clone https://github.com/cktan/tomlc99.git "$SCRIPT_DIR"/../deps/tomlc99
fi
# install zig dependency for zig plugin.
if [ ! -d "$SCRIPT_DIR"/../deps/utf8-zig ]; then
  git clone https://github.com/jmatth11/utf8-zig.git "$SCRIPT_DIR"/../deps/utf8-zig
fi
if [ ! -d "$SCRIPT_DIR"/../deps/scribe ]; then
  git clone https://github.com/jmatth11/scribe.git "$SCRIPT_DIR"/../deps/scribe
fi

"$SCRIPT_DIR"/build_deps.sh $1
