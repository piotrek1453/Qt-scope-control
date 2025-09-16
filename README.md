# Qt-scope-control

User-friendly GUI app for oscilloscope class instrument control using VISA created for engineering thesis project.

# Features

## Basic

- Connecting to instrument specified by user through any communication means supported by VISA,
- Basic instrument control using GUI.

### Control functions

- Autoscale,
- Horizontal scale,
- Vertical scale,
- Horizontal offset,
- Vertical offset,
- Acquisition mode,
- Frequency measurement,
- VRMS measurement.

## Bells and whistles

- Control of instruments from several manufacturers - commands mapped to generic operations in yaml file. Examples in modules/CommandParser.

# Building

## Requirements

- C++ toolchain (tested on GCC in Linux and the one bundled with mingw-w64),
- CMake,
- Ninja,
- Qt,
- VISA implementation (tested on NI-VISA, others might require tweaking CMakeLists.txt).

## Using scripts

Simply run one of build_and_run scripts for corresponding platform.
