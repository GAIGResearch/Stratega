# CMakeList.txt : CMake project for StrategyGameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable (Optimization ${sources})
target_include_directories(Optimization PUBLIC include)
target_link_libraries(Optimization Stratega)