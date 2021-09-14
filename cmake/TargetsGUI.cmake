
set(GUI_SOURCE_FILES
        main.cpp
        )

list(TRANSFORM GUI_SOURCE_FILES PREPEND "${GUI_SRC_DIR}/")

add_executable (gui ${GUI_SOURCE_FILES})

target_include_directories(gui PUBLIC ${STRATEGA_INCLUDE_DIR})
target_link_libraries(gui PUBLIC stratega)

#Copy Assets folder
file(COPY ${GUI_DIR}/Assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message("$<$<PLATFORM_ID:Linux>:sfml-system>")
target_link_libraries(gui
        PRIVATE
        # other platforms use Conan's `sfml`
        "$<$<NOT:$<PLATFORM_ID:Linux>>:CONAN_PKG::sfml>"
        # for linux we have to use the targets of `sfml`'s components individually
        "$<$<PLATFORM_ID:Linux>:sfml-system>"
        "$<$<PLATFORM_ID:Linux>:sfml-graphics>"
        "$<$<PLATFORM_ID:Linux>:sfml-window>"
        )
