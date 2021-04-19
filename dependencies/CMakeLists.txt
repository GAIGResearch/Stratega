include(FetchContent)

set(SFML_VERSION 2.5.1)
set(YAML_CPP_VERSION 0.6.3)
set(PYBIND11_VERSION 2.6.2)

message(STATUS "Fetching third party libraries")
# ----- START: Fetch dependencies -----
FetchContent_Declare (
	sfml
	URL "https://github.com/SFML/SFML/archive/${SFML_VERSION}.zip"
)

FetchContent_Declare (
	yaml_cpp
	GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp.git"
)

FetchContent_Declare (
	pybind11
	URL "https://github.com/pybind/pybind11/archive/refs/tags/v${PYBIND11_VERSION}.zip"
)

add_subdirectory("sfml")
add_subdirectory("yaml-cpp")
add_subdirectory("imgui")
add_subdirectory("recastNavigation")
add_subdirectory("pybind11")
# ----- END: Fetch dependencies -----
message(STATUS "Fetching third party libraries - done")