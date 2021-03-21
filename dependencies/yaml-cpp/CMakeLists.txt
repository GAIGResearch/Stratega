FetchContent_GetProperties(yaml_cpp)

if(NOT yaml_cpp_POPULATED)
	FetchContent_Populate(yaml_cpp)

	set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "disable yaml tests")
	set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "disable yaml tools")
	set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "disable yaml contrib")
	set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "disable shared libs")
	add_subdirectory(${yaml_cpp_SOURCE_DIR} ${yaml_cpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()