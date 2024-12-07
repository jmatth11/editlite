# install core dependencies.
sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev
git clone https://github.com/jmatth11/array_template.git deps/array_template
git clone https://github.com/cktan/tomlc99.git deps/tomlc99
# install zig dependency for zig plugin.
sudo apt install -y snapd
sudo snap install zig --classic --beta
git clone https://github.com/jmatth11/utf8-zig.git deps/utf8-zig

