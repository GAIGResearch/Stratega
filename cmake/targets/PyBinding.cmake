set(BINDING_SOURCE_FILES
        binding.cpp
        )

list(TRANSFORM BINDING_SOURCE_FILES PREPEND "${SUBPROJ_PYBINDING_SRC_DIR}/")

set(PYBIND11_FINDPYTHON TRUE)
if(DEFINED ${pybind11_SEARCH_PATH})
    find_package(pybind11 CONFIG PATHS ${pybind11_SEARCH_PATH} REQUIRED)
else()
    find_package(pybind11 REQUIRED)
endif()

pybind11_add_module(py${project_lib_name} ${BINDING_SOURCE_FILES})

target_link_libraries(py${project_lib_name} PUBLIC ${project_lib_name})
set_target_properties(py${project_lib_name} PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE
         LIBRARY_OUTPUT_DIRECTORY ${SUBPROJ_PYBINDING_DIR}
         OUTPUT_NAME ${PROJECT_NAME_LOWERCASE}
        )