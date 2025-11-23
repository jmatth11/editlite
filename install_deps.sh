# install core dependencies.
git clone https://github.com/jmatth11/array_template.git deps/array_template
git clone https://github.com/cktan/tomlc99.git deps/tomlc99
cd deps/tomlc99
make
cd -
# install zig dependency for zig plugin.
git clone https://github.com/jmatth11/utf8-zig.git deps/utf8-zig
cd deps/utf8-zig
make
cd -

git clone https://github.com/jmatth11/scribe.git deps/scribe
cd deps/scribe
make
cd -

