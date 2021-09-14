
set(ARENA_SOURCE_FILES
        main.cpp
        )

list(TRANSFORM ARENA_SOURCE_FILES PREPEND "${ARENA_SRC_DIR}/")

add_executable (arena ${ARENA_SOURCE_FILES})

target_include_directories(arena PUBLIC ${STRATEGA_INCLUDE_DIR})
target_link_libraries(arena PUBLIC stratega)