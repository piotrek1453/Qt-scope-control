cmake_minimum_required(VERSION 3.27)

project(Qtscopecontrol)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(InstrumentControl)
add_subdirectory(OscilloscopeGUI)
add_subdirectory(CommandParser)
