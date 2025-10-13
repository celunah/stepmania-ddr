cmake_minimum_required(VERSION 3.10...4.0)

project(OpenDDR)

include(StepmaniaCore.cmake)

# The external libraries need to be included.
add_subdirectory(extern)

# The internal libraries and eventual executable to be used.
add_subdirectory(src)
