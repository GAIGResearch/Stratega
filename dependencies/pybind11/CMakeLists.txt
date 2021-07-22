set(PYBIND11_VERSION 2.6.2)
FetchContent_Declare (
	pybind11
	URL "https://github.com/pybind/pybind11/archive/refs/tags/v${PYBIND11_VERSION}.zip"
)
FetchContent_GetProperties(pybind11)

if(NOT pybind11_POPULATED)
	FetchContent_Populate(pybind11)

	add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()