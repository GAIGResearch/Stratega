set(YAML_CPP_VERSION 0.6.3)
FetchContent_Declare (
	yaml_cpp
	URL "https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-${YAML_CPP_VERSION}.zip"
)
FetchContent_GetProperties(yaml_cpp)

if(NOT yaml_cpp_POPULATED)
	FetchContent_Populate(yaml_cpp)

	set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "disable yaml tests")
	set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "disable yaml tools")
	set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "disable yaml contrib")
	set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "enable shared libs")
	add_subdirectory(${yaml_cpp_SOURCE_DIR} ${yaml_cpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()