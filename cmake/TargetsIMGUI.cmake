set(IMGUI_SOURCE_FILES
        imgui-SFML.cpp
        )

list(TRANSFORM IMGUI_SOURCE_FILES PREPEND "${imgui-sfml_SOURCE_DIR}/")

add_library(ImGUI STATIC ${IMGUI_SOURCE_FILES})
target_include_directories(ImGUI PUBLIC ${imgui-sfml_SOURCE_DIR})
target_link_libraries(ImGUI
        PRIVATE
        CONAN_PKG::imgui
        # other platforms use Conan's `sfml`
        "$<$<NOT:$<PLATFORM_ID:Linux>>:CONAN_PKG::sfml>"
        # for linux we have to use the targets of `sfml`'s components individually
        "$<$<PLATFORM_ID:Linux>:sfml-system>"
        "$<$<PLATFORM_ID:Linux>:sfml-graphics>"
        "$<$<PLATFORM_ID:Linux>:sfml-window>"
        OpenGL
        )