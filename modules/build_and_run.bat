REM create build directory
mkdir build
REM change directory to build
cd build
REM generate files for build type Release with shared libs off, 
REM embed libraries into executable (specific to MinGW)
cmake -GNinja -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ^
-DCMAKE_CXX_FLAGS="-static -static-libgcc -static-libstdc++ -lwinpthread" ..
REM build the project
cmake --build .
REM run the executable
cd OscilloscopeGUI
OscilloscopeGUI.exe
