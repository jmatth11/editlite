#!/bin/bash

which appimagetool || exit 1
./install_deps.sh || exit 1
make || exit 1
mkdir -p editlite.AppDir/usr/bin
mkdir -p editlite.AppDir/lib
cp release_info/AppDir editlite.AppDir
cp release_info/editlite.desktop editlite.AppDir
cp bin/editlite editlite.AppDir/usr/bin/
SDL_LIB=$(shell find /usr/lib -name libSDL2.so)
SDL_TTF_LIB=$(shell find /usr/lib -name libSDL2_ttf.so)
cp "$SDL_LIB" editlite.AppDir/lib/
cp "$SDL_TTF_LIB" editlite.AppDir/lib/
ARCH=x86_64 appimagetool editlite.AppDir
