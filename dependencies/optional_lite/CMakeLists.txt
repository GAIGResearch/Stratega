#set(OPTIONAL_LITE_VERSION 3.4.0)
#FetchContent_Declare (
#	optional_lite
#	URL "https://github.com/martinmoene/optional-lite/archive/refs/tags/v${OPTIONAL_LITE_VERSION}.zip"
#)
#FetchContent_GetProperties(optional_lite)
#
#if(NOT optional_lite_POPULATED)
#	FetchContent_Populate(optional_lite)
#	set(OPTIONAL_LITE_OPT_BUILD_TESTS CACHE BOOL FALSE)
#	add_subdirectory(${optional_lite_SOURCE_DIR} ${optional_lite_BINARY_DIR} EXCLUDE_FROM_ALL)
#endif()

# Add library
#file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.hpp")
#add_library(optionalLite STATIC ${sources})
#set_target_properties(optionalLite PROPERTIES LINKER_LANGUAGE C)
#target_link_libraries(optionalLite)
#target_include_directories(optionalLite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})