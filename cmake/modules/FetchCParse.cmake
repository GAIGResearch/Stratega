#include(FetchContent)
#
#FetchContent_Declare (
#		cparse
#		GIT_REPOSITORY https://github.com/cparse/cparse.git
#)
#
#FetchContent_GetProperties(cparse)
#
#if(NOT cparse_POPULATED)
#	FetchContent_Populate(cparse)
#endif()
#
#add_library(cparse INTERFACE)
#target_include_directories(cparse INTERFACE ${cparse_SOURCE_DIR})
#