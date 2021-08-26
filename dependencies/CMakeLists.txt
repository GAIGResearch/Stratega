include(FetchContent)

message(STATUS "Fetching third party libraries")

add_subdirectory("sfml")
add_subdirectory("yaml-cpp")
add_subdirectory("imgui")
add_subdirectory("recastNavigation")
#add_subdirectory("optional_lite")

add_subdirectory("pybind11")

message(STATUS "Fetching third party libraries - done")
