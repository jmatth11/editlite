sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev
git clone https://github.com/jmatth11/array_template.git deps/array_template
git clone https://github.com/cktan/tomlc99.git deps/tomlc99

cd deps/tomlc99/
make
