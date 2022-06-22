set(GUI_SOURCE_FILES
        main.cpp
        )

list(TRANSFORM GUI_SOURCE_FILES PREPEND "${SUBPROJ_GUI_SRC_DIR}/")

add_executable (gui ${GUI_SOURCE_FILES})
target_include_directories(gui PUBLIC ${SUBPROJ_STRATEGA_INCLUDE_DIR})


if(SGA_BUILD_SFML_FROM_SOURCE OR CMAKE_SYSTEM_NAME MATCHES Linux)
   target_link_libraries(gui
    PUBLIC
    Stratega
    PRIVATE
    # other platforms use Conan's `sfml`
    # for mac we have to use the targets of `sfml`'s components individually
    #"$<$<PLATFORM_ID:Windows>:CONAN_PKG::sfml>"

    sfml-system
    sfml-graphics
    sfml-window

    ## for linux we have to use the targets of `sfml`'s components individually
    #"$<$<PLATFORM_ID:Linux>:sfml-system>"
    #"$<$<PLATFORM_ID:Linux>:sfml-graphics>"
    #"$<$<PLATFORM_ID:Linux>:sfml-window>"
    )
else()
    target_link_libraries(gui
    PUBLIC
    Stratega
    PRIVATE
    # other platforms use Conan's `sfml`
    # for mac we have to use the targets of `sfml`'s components individually
    #"$<$<PLATFORM_ID:Windows>:CONAN_PKG::sfml>"
    #
    #"$<$<PLATFORM_ID:Darwin>:sfml-system>"
    #"$<$<PLATFORM_ID:Darwin>:sfml-graphics>"
    #"$<$<PLATFORM_ID:Darwin>:sfml-window>"
    #
    ## for linux we have to use the targets of `sfml`'s components individually
    #"$<$<PLATFORM_ID:Linux>:sfml-system>"
    #"$<$<PLATFORM_ID:Linux>:sfml-graphics>"
    #"$<$<PLATFORM_ID:Linux>:sfml-window>"

    CONAN_PKG::sfml
    )
endif()




set_target_properties(gui PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/lib
         RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/bin
         ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/lib
         EXECUTABLE_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/bin

         LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_DIRECTORY}
         RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RUNTIME_OUTPUT_DIRECTORY}
         ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${ARCHIVE_OUTPUT_DIRECTORY}
         EXECUTABLE_OUTPUT_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_DIRECTORY}
         
         LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_DIRECTORY}
         RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RUNTIME_OUTPUT_DIRECTORY}
         ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${ARCHIVE_OUTPUT_DIRECTORY}
         EXECUTABLE_OUTPUT_DIRECTORY_DEBUG ${EXECUTABLE_OUTPUT_DIRECTORY}
        )
