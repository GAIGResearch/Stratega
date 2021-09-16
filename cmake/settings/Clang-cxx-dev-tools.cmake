file(GLOB_RECURSE
        ALL_CXX_SOURCE_FILES
        ${PROJ_SOURCES_DIR}/**.[ch]pp
        ${PROJ_SOURCES_DIR}/**.h
        )
# Adding clang-format target if executable is found
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    add_custom_target(
            clang-format
            COMMAND ${CLANG_FORMAT}
            -i
            -style=file
            ${ALL_CXX_SOURCE_FILES}
    )
endif()

# Adding clang-tidy target if executable is found
find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
    add_custom_target(
            clang-tidy
            COMMAND ${CLANG_TIDY}
            ${ALL_CXX_SOURCE_FILES}
            -config=''
            --
            -std=${PROJ_CXX_STANDARD}
            ${ALL_CXX_SOURCE_FILES}
    )
endif()