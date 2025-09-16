#!/usr/bin/sh

# create build directory
mkdir -p build
# change directory to build
cd build
# generate files for build type Release with shared libs off
cmake -GNinja -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
# build the project
cmake --build .
# run the executable
./OscilloscopeGUI/OscilloscopeGUI
