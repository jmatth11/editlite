#!/bin/bash

sudo apt-get install libfuse2
which wget
if [[ $? != 0 ]]; then
  sudo apt-get install -y wget
fi
wget "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage" || exit 1
chmod a+x appimagetool-x86_64.AppImage || exit 1
./install_deps.sh
make || exit 1
mkdir -p editlite.AppDir/usr/bin
mkdir -p editlite.AppDir/lib
cp release_files/AppRun editlite.AppDir
cp release_files/editlite.desktop editlite.AppDir
cp bin/editlite editlite.AppDir/usr/bin/
SDL_LIB=$(find /usr/lib -name libSDL2.so)
SDL_TTF_LIB=$(find /usr/lib -name libSDL2_ttf.so)
cp "$SDL_LIB" editlite.AppDir/lib/
cp "$SDL_TTF_LIB" editlite.AppDir/lib/
ARCH=x86_64 ./appimagetool-x86_64.AppImage editlite.AppDir || exit 1
rm appimagetool-x86_64.AppImage
