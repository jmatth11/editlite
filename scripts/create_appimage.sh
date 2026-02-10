#!/usr/bin/env bash

# TODO see if this script still works.

function clean_up {
  rm appimagetool-x86_64.AppImage 2> /dev/null
  rm -rf editlite.AppDir 2> /dev/null
}

# clean up on script exit
trap clean_up EXIT

echo "installing dependencies..."
sudo apt-get install -y libfuse2 wget
wget "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage" || exit 1
chmod a+x appimagetool-x86_64.AppImage || exit 1
./install_deps.sh

echo ""
echo "building program..."
make || exit 1

echo ""
echo "creating AppDir"
mkdir -p editlite.AppDir/usr/bin
mkdir -p editlite.AppDir/lib

echo "copying files over..."
cp release_files/AppRun editlite.AppDir
cp release_files/editlite.desktop editlite.AppDir
cp release_files/editlite-icon.png editlite.AppDir
cp bin/editlite editlite.AppDir/usr/bin/

echo ""
echo "finding libraries..."
SDL_LIB=$(find /usr/lib -name libSDL2.so)
SDL_TTF_LIB=$(find /usr/lib -name libSDL2_ttf.so)

echo "copying libraries..."
cp "$SDL_LIB" editlite.AppDir/lib/
cp "$SDL_TTF_LIB" editlite.AppDir/lib/

echo "creating AppImage"
ARCH=x86_64 ./appimagetool-x86_64.AppImage editlite.AppDir || exit 1
