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
	set(BUILD_SHARED_LIBS TRUE)
	add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()