find_package(GTest REQUIRED)


set(TEST_SOURCES
        main_test.cpp
        )

list(TRANSFORM TEST_SOURCES PREPEND "${PROJ_TEST_FOLDER}/")

add_executable(tests ${TEST_SOURCES})

set_target_properties(tests PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        EXCLUDE_FROM_ALL True  # don't build tests when ALL is asked to be built. Only on demand.
        )

target_link_libraries(tests PRIVATE
        Stratega
        CONAN_PKG::gtest)

