#cmake_minimum_required (VERSION 3.8)
#
#
## Build the Google Test library
#add_subdirectory(gtest)
#
#set(BINARY ${CMAKE_PROJECT_NAME}_Test)
#
#file(GLOB_RECURSE TEST_SOURCES "src/*.cpp")
#
#add_executable(
#	${BINARY}
#	${TEST_SOURCES}
#)
#add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
#
#target_include_directories (
#    ${BINARY}
#    PUBLIC
#    ${CMAKE_CURRENT_SOURCE_DIR}/src
#    ${CMAKE_CURRENT_SOURCE_DIR}/../src
#)
#
## Link against our built libraries
#target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME} yaml-cpp GTest::gtest GTest::gmock)