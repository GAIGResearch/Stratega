# CMakeList.txt : CMake project for StrategyGameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable (Arena ${sources})
target_include_directories(Arena PUBLIC include)
target_link_libraries(Arena Stratega)