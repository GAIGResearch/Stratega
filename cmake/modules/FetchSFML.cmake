include(FetchContent)

set(SFML_VERSION 2.5.1)
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
endif()
