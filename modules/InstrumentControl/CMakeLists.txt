cmake_minimum_required(VERSION 3.27)
project(InstrumentControl)

add_library(InstrumentControl src/InstrumentControl.cpp
                              inc/InstrumentControl.hpp)
target_include_directories(InstrumentControl
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

if(WIN32)
  find_library(
    VISA_LIB REQUIRED
    NAMES visa64
    HINTS "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Lib_x64/msc")
  target_include_directories(
    InstrumentControl
    PUBLIC "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Include")
  target_link_libraries(InstrumentControl PUBLIC "${VISA_LIB}")
endif(WIN32)

if(UNIX)
  find_library(
    VISA_LIB REQUIRED
    NAMES visa
    HINTS "/usr/include")
  target_include_directories(InstrumentControl PUBLIC "/usr/include/ni-visa")
  target_include_directories(InstrumentControl PUBLIC "/usr/include")
  target_link_libraries(InstrumentControl PUBLIC visa)
endif(UNIX)

include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.x
  GIT_SHALLOW FALSE # ensure submodules are checked out
  OVERRIDE_FIND_PACKAGE)
FetchContent_MakeAvailable(spdlog)

target_link_libraries(InstrumentControl PUBLIC spdlog::spdlog)
