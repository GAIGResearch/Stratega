# CMakeList.txt : CMake project for StrategyGameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add library
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp" "*.h")
add_library(ImGUI STATIC ${sources})
target_link_libraries(ImGUI
	PRIVATE
		sfml-graphics
		sfml-system
		sfml-window
		OpenGL)
target_include_directories(ImGUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})