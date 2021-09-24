cmake_minimum_required (VERSION 3.13)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp" "*.h")
add_executable (GUI ${sources})

target_include_directories(GUI PUBLIC include)
target_link_libraries(GUI PUBLIC Stratega)


target_link_libraries(GUI
	PRIVATE
		sfml-graphics
		sfml-system
		sfml-window
		ImGUI)
