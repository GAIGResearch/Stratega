cmake_minimum_required (VERSION 3.13)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp" "*.h")
add_executable (GUI ${sources})

target_include_directories(GUI PUBLIC include)
target_link_libraries(GUI PUBLIC Stratega)

#Copy Assets folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Assets
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(GUI
	PRIVATE
		sfml-graphics
		sfml-system
		sfml-window
		ImGUI)
