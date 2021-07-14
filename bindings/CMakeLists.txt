cmake_minimum_required (VERSION 3.8)

pybind11_add_module(stratega ./python/python.cpp)
target_link_libraries(stratega PUBLIC Stratega)