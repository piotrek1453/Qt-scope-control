#!/usr/bin/sh

set -e

# create build directory
mkdir -p build
# change directory to build
cd build
# generate files for build type Release with shared libs off
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
# build the project
cmake --build .
# run the executable
./OscilloscopeGUI/OscilloscopeGUI
