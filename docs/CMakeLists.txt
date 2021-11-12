# Add the cmake folder so the FindSphinx and FindDoxygen module is found
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

# Find all the public headers
get_target_property(STRATEGA_PUBLIC_HEADER_DIR Stratega INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE STRATEGA_PUBLIC_HEADERS ${STRATEGA_PUBLIC_HEADER_DIR}/*.h)

set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/Stratega/include)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${STRATEGA_PUBLIC_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating docs"
                   VERBATIM)

# Nice named target so we can run the job easily
add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

#if(STRATEGA_BUILD_BINDINGS)
#    message(STATUS "Needs building first")
#    #target_link_libraries(Doxygen PUBLIC stratega)
#    add_dependencies(Doxygen stratega)
#endif()

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

# Collect all .rst files
file(GLOB_RECURSE DOCS_RST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.rst)

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND 
                     ${SPHINX_EXECUTABLE} -b html
                     # Tell Breathe where to find the Doxygen output
                     -Dbreathe_projects.Stratega=${DOXYGEN_OUTPUT_DIR}/xml
                   ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS
                   # Other docs files you want to track should go here (or in some variable)
                   ${DOCS_RST_FILES}
                   ${DOXYGEN_INDEX_FILE}
                   MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                   COMMENT "Generating documentation with Sphinx")

# Nice named target so we can run the job easily
add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

#if(STRATEGA_BUILD_BINDINGS)
#    message(STATUS "Needs building first")
#    add_dependencies(Sphinx stratega)
#endif()