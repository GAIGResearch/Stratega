
set(ARENA_SOURCE_FILES
        main.cpp
        )

list(TRANSFORM ARENA_SOURCE_FILES PREPEND "${SUBPROJ_ARENA_SRC_DIR}/")

add_executable (arena ${ARENA_SOURCE_FILES})

target_include_directories(arena PUBLIC ${SUBPROJ_STRATEGA_INCLUDE_DIR})
target_link_libraries(arena PUBLIC stratega)