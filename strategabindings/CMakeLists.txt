cmake_minimum_required (VERSION 3.8)

pybind11_add_module(strategabindings ./python/python.cpp)
target_link_libraries(strategabindings PUBLIC Stratega)

set_target_properties(strategabindings PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}../../../bindings)