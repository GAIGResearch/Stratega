# CMakeList.txt : CMake project for StrategyGameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add library
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(Stratega STATIC ${sources})

target_include_directories(Stratega PUBLIC include)
target_link_libraries(Stratega PUBLIC yaml-cpp)
target_link_libraries(Stratega PUBLIC Recast)
target_link_libraries(Stratega PUBLIC Detour)
target_link_libraries(Stratega PUBLIC DebugUtils)


# ToDo disable yaml-cpp warnings so we can enable this
#target_compile_options(Stratega PRIVATE
#  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
#  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
#)
