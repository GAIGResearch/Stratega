cmake_minimum_required (VERSION 3.8)

pybind11_add_module(stratega                     
                    ./src/python.cpp)
target_link_libraries(stratega PUBLIC Stratega)

#set_target_properties(stratega PROPERTIES
#         LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}../../../bindings)