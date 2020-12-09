# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Stratega")


# set(CMAKE_VERBOSE_MAKEFILE on)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#add_compile_options("/std:c++latest")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

if (WIN32 OR UNIX)
    set(COMPILER_FLAGS "")
elseif (APPLE)
    set(COMPILER_FLAGS "-Xclang -fconcepts-ts")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS}")

# Include sub-projects.
add_subdirectory("dependencies")
add_subdirectory("Stratega")
add_subdirectory("Arena")
add_subdirectory("GUI")
add_subdirectory("Tests")