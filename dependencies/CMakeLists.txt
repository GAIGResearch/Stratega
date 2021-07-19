include(FetchContent)

message(STATUS "Fetching third party libraries")

add_subdirectory("sfml")
add_subdirectory("yaml-cpp")
add_subdirectory("imgui")
add_subdirectory("recastNavigation")
if(STRATEGA_BUILD_BINDINGS)
	add_subdirectory("pybind11")
endif()

message(STATUS "Fetching third party libraries - done")