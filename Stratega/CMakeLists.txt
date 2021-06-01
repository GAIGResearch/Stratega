# CMakeList.txt : CMake project for StrategyGameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add library
file(GLOB_RECURSE STRATEGA_SOURCE_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(Stratega STATIC ${STRATEGA_SOURCE_FILES})

target_include_directories(Stratega PUBLIC include)
target_link_libraries(Stratega PUBLIC yaml-cpp)
target_link_libraries(Stratega PUBLIC Recast)
target_link_libraries(Stratega PUBLIC Detour)
target_link_libraries(Stratega PUBLIC DebugUtils)
target_link_libraries(Stratega
	PRIVATE
		sfml-graphics
		sfml-system
		sfml-window
		ImGUI)

install (TARGETS Stratega yaml-cpp
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)

install(DIRECTORY include DESTINATION .)
get_target_property(YAML_CPP_INCLUDE_DIRECTORIES yaml-cpp INCLUDE_DIRECTORIES)
install(DIRECTORY ${YAML_CPP_INCLUDE_DIRECTORIES} DESTINATION .)


target_compile_options(Stratega PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)
string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
