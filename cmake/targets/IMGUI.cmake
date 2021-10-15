set(IMGUI_SOURCE_FILES
        imgui-SFML.cpp
        )

list(TRANSFORM IMGUI_SOURCE_FILES PREPEND "${imgui-sfml_SOURCE_DIR}/")

add_library(imgui STATIC ${IMGUI_SOURCE_FILES})
target_include_directories(imgui PUBLIC ${imgui-sfml_SOURCE_DIR})
target_link_libraries(imgui
        PRIVATE
        CONAN_PKG::imgui
        # other platforms use Conan's `sfml`
        "$<$<NOT:$<PLATFORM_ID:Linux>>:CONAN_PKG::sfml>"
        "$<$<PLATFORM_ID:Linux>:OpenGL>"
        "$<$<PLATFORM_ID:Windows>:CONAN_PKG::opengl>"
        #"$<$<NOT:$<PLATFORM_ID:Linux>>:CONAN_PKG::opengl>"
        # for linux we have to use the targets of `sfml`'s components individually
        "$<$<PLATFORM_ID:Linux>:sfml-system>"
        "$<$<PLATFORM_ID:Linux>:sfml-graphics>"
        "$<$<PLATFORM_ID:Linux>:sfml-window>"
        #CONAN_PKG::opengl        
        )