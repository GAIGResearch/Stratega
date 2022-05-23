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
        "$<$<PLATFORM_ID:Darwin>:CONAN_PKG::opengl>"
        #OpenGL
        "$<$<PLATFORM_ID:Linux>:CONAN_PKG::opengl>"
        "$<$<PLATFORM_ID:Windows>:CONAN_PKG::opengl>"
        #"$<$<PLATFORM_ID:Windows>:CONAN_PKG::sfml>"
        #CONAN_PKG::sfml
        #"$<$<NOT:$<PLATFORM_ID:Linux>>:CONAN_PKG::opengl>"
        # for linux we have to use the targets of `sfml`'s components individually
        "$<$<PLATFORM_ID:Linux>:sfml-system>"
        "$<$<PLATFORM_ID:Linux>:sfml-graphics>"
        "$<$<PLATFORM_ID:Linux>:sfml-window>"
        
        "$<$<PLATFORM_ID:Darwin>:sfml-system>"
        "$<$<PLATFORM_ID:Darwin>:sfml-graphics>"
        "$<$<PLATFORM_ID:Darwin>:sfml-window>"

        sfml-system
        sfml-graphics
        sfml-window
        #CONAN_PKG::opengl        
        )