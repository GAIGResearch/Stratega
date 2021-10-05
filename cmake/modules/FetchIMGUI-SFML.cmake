include(FetchContent)

set(IMGUI-SFML_VERSION 2.3)
FetchContent_Declare (
		imgui-sfml
		URL "https://github.com/eliasdaler/imgui-sfml/archive/refs/tags/v${IMGUI-SFML_VERSION}.zip"
)
FetchContent_GetProperties(sfml)
if(NOT imgui-sfml_POPULATED)
	FetchContent_Populate(imgui-sfml)
	set(IMGUI_SFML_FIND_SFML FALSE)
#	set(imgui-sfml_SOURCE_DIR ${imgui-sfml_SOURCE_DIR} PARENT_SCOPE)
#	set(imgui-sfml_BINARY_DIR ${imgui-sfml_BINARY_DIR} PARENT_SCOPE)
endif()
