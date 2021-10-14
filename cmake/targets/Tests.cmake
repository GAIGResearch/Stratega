
find_package(GTest REQUIRED)

set(TEST_SOURCES
        main_test.cpp
        )

list(TRANSFORM TEST_SOURCES PREPEND "${PROJ_TEST_FOLDER}/")

add_executable(${project_lib_name}_tests ${TEST_SOURCES})

set_target_properties(${project_lib_name}_tests PROPERTIES
        EXCLUDE_FROM_ALL True  # don't build tests when ALL is asked to be built. Only on demand.
        )

target_link_libraries(${project_lib_name}_tests PRIVATE
        ${project_lib_name}
        CONAN_PKG::gtest)

add_test(NAME ${project_lib_name}_CTEST COMMAND ${project_lib_name}_tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

