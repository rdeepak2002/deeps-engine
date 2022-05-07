#!/bin/sh

# allow modification of this folder
sudo chmod -R 777 ./

# download emscripten
./download-emscripten.sh
source emsdk/emsdk_env.sh

# install vcpkg stuff
./download-vcpkg.sh
export VCPKG_ROOT="$(pwd)/vcpkg"
./vcpkg/vcpkg install lua:wasm32-emscripten

echo "Creating web library..."

# remove current library code
rm -rf web-library

# build source code
export AS_LIBRARY=""
cmake -S ../../.. -B web-library "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=$(pwd)/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" "-DCMAKE_TOOLCHAIN_FILE=$(pwd)/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=wasm32-emscripten"
cmake --build web-library

# copy compiled library to engine lib folder
mkdir -p ../../engine/lib/web
cp web-library/libDeepsEngine.a ../../engine/lib/web/libDeepsEngine.a

# serve content
echo "Done building emscripten version of DeepsEngine library in web-library folder"