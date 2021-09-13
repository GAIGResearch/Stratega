cmake_minimum_required (VERSION 3.8)

set(PYBIND11_FINDPYTHON TRUE)

if(DEFINED ${pybind11_SEARCH_PATH})
    find_package(pybind11 CONFIG PATHS ${pybind11_SEARCH_PATH} REQUIRED)
else()
    find_package(pybind11 REQUIRED)
endif()

pybind11_add_module(pystratega ../bindings/python/python.cpp)
target_link_libraries(pystratega PUBLIC stratega)

set_target_properties(pystratega PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ${STRATEGA_BINDINGS_DIR})