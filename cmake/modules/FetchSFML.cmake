include(FetchContent)

set(SFML_VERSION 2.6.0)
FetchContent_Declare (
	sfml
	#URL "https://github.com/SFML/SFML/archive/${SFML_VERSION}.zip"
	URL "https://codeload.github.com/SFML/SFML/zip/refs/heads/2.6.x"
)
FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
	FetchContent_Populate(sfml)
	set(SFML_BUILD_NETWORK CACHE BOOL FALSE)
	set(SFML_BUILD_AUDIO CACHE BOOL FALSE)
	set(SFML_STATIC_LIBRARIES FALSE)
#	set(imgui-sfml_SOURCE_DIR ${sfml_SOURCE_DIR} PARENT_SCOPE)
#	set(imgui-sfml_BINARY_DIR ${sfml_BINARY_DIR} PARENT_SCOPE)
endif()