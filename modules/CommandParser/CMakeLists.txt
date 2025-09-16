cmake_minimum_required(VERSION 3.27)

project(CommandParser)

if(RYML_BRANCH_NAME STREQUAL "")
  find_program(GIT git REQUIRED)
  execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    OUTPUT_VARIABLE RYML_BRANCH_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
if(RYML_BRANCH_NAME STREQUAL HEAD)
  set(RYML_BRANCH_NAME master)
endif()
message(STATUS "FetchContent from branch: ${RYML_BRANCH_NAME}")

include(FetchContent)
FetchContent_Declare(
  ryml
  GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
  GIT_TAG ${RYML_BRANCH_NAME}
  GIT_SHALLOW FALSE # ensure submodules are checked out
)
FetchContent_MakeAvailable(ryml)

file(GLOB COMMAND_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.yml")
file(COPY ${COMMAND_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/..)
file(COPY ${COMMAND_FILES}
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../OscilloscopeGUI)
file(COPY ${COMMAND_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_library(CommandParser src/CommandParser.cpp inc/CommandParser.hpp)

target_include_directories(CommandParser
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

target_link_libraries(CommandParser ryml::ryml)
