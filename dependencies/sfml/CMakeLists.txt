FetchContent_GetProperties(sfml)

if(NOT sfml_POPULATED)
	FetchContent_Populate(sfml)

	set(SFML_BUILD_NETWORK CACHE BOOL FALSE)
	set(SFML_BUILD_AUDIO CACHE BOOL FALSE)
	add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()