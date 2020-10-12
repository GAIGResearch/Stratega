include(FetchContent)

set(SFML_VERSION 2.5.1)
set(YAML_CPP_VERSION 0.6.3)

message(STATUS "Fetching third party libraries")
# ----- START: Fetch dependencies -----
FetchContent_Declare (
	sfml
	URL "https://github.com/SFML/SFML/archive/${SFML_VERSION}.zip"
)

FetchContent_Declare (
	yaml_cpp
	URL "https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-${YAML_CPP_VERSION}.zip"
)

add_subdirectory("sfml")
add_subdirectory("yaml-cpp")
add_subdirectory("imgui")
# ----- END: Fetch dependencies -----
message(STATUS "Fetching third party libraries - done")