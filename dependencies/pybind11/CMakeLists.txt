FetchContent_GetProperties(pybind11)

if(NOT pybind11_POPULATED)
	FetchContent_Populate(pybind11)

	add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()