include(FetchContent)

set(SFML_VERSION 2.5.1)
FetchContent_Declare (
	sfml
	URL "https://github.com/SFML/SFML/archive/${SFML_VERSION}.zip"
)
FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
	FetchContent_Populate(sfml)
	set(SFML_BUILD_NETWORK CACHE BOOL FALSE)
	set(SFML_BUILD_AUDIO CACHE BOOL FALSE)
	set(SFML_STATIC_LIBRARIES TRUE)
#	set(imgui-sfml_SOURCE_DIR ${sfml_SOURCE_DIR} PARENT_SCOPE)
#	set(imgui-sfml_BINARY_DIR ${sfml_BINARY_DIR} PARENT_SCOPE)
endif()