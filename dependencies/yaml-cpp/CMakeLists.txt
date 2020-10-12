FetchContent_GetProperties(yaml_cpp)

if(NOT yaml_cpp_POPULATED)
	FetchContent_Populate(yaml_cpp)

	add_subdirectory(${yaml_cpp_SOURCE_DIR} ${yaml_cpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()