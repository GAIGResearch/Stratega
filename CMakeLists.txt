# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Stratega")

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_compile_options("/std:c++latest")

# Include sub-projects.
add_subdirectory("dependencies")
add_subdirectory("Stratega")
add_subdirectory("Arena")
add_subdirectory("GUI")
add_subdirectory("Tests")