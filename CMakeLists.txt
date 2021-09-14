cmake_minimum_required(VERSION 3.15)

project(Stratega LANGUAGES CXX)

# project set'tings
set(ARENA_SRC_DIR "${CMAKE_SOURCE_DIR}/src/Arena")
set(CMAKE_CONFIG_FOLDER "cmake")
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
if (CMAKE_SYSTEM_NAME MATCHES Windows)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif ()
if (CMAKE_SYSTEM_NAME MATCHES Linux)
    set(CONANFILE conanfile_linux.txt)
else ()
    set(CONANFILE conanfile.txt)
endif ()
set(DEPENDENCY_DIR deps)
set(GUI_DIR ${CMAKE_SOURCE_DIR}/src/GUI)
set(GUI_SRC_DIR ${GUI_DIR}/src)
set(project_cxx_standard cxx_std_20)
set(project_CPP_STANDARD C++20)
set(STRATEGA_BINDINGS_DIR ${CMAKE_SOURCE_DIR}/src/bindings)
set(STRATEGA_DIR "${CMAKE_SOURCE_DIR}/src/Stratega")
set(STRATEGA_INCLUDE_DIR "${STRATEGA_DIR}/include")
set(STRATEGA_SRC_DIR "${STRATEGA_DIR}/src")
set(STRATEGA_TEST_FOLDER "${CMAKE_SOURCE_DIR}/test")

# options

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)
option(ENABLE_FUZZING "Enable Fuzzing Builds" OFF)
option(ENABLE_IPO "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)" OFF)
option(ENABLE_PCH "Enable Precompiled Headers" ON)
option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer" OFF)
option(ENABLE_TESTING "Enable Test Builds" ON)
option(STRATEGA_BUILD_DOCS "Enable to build the docs. Requires doxygen to be installed on the system" OFF)
option(STRATEGA_BUILD_BINDINGS "Enable to build the bindings to other languages." OFF)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

# print out a short summary of the compiler information

message("C++ Compiler: ${CMAKE_CXX_COMPILER}")
message("C++ Compiler Info: ${CMAKE_CXX_COMPILER_ID}, version ${CMAKE_CXX_COMPILER_VERSION}")
message("CURRENT CMAKE BINARY DIR: ${CMAKE_CURRENT_BINARY_DIR}")

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    if (ENABLE_BUILD_WITH_TIME_TRACE)
        add_compile_definitions(project_options INTERFACE -ftime-trace)
    endif ()
endif ()

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

target_compile_features(project_options INTERFACE ${project_cxx_standard})
if (ENABLE_PCH)
    target_precompile_headers(project_options
            INTERFACE
            <vector>
            <string>
            <map>
            <utility>
            <memory>
            <array>
            <optional>
            )
endif ()

# import utility methods for cmake
include(${CMAKE_CONFIG_FOLDER}/settings/Utilities.cmake)

#enable clang-format and clang-tidy project wide
include(${CMAKE_CONFIG_FOLDER}/settings/Clang-cxx-dev-tools.cmake)

# enable cache system
include(${CMAKE_CONFIG_FOLDER}/settings/Cache.cmake)

# standard compiler warnings
include(${CMAKE_CONFIG_FOLDER}/settings/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(${CMAKE_CONFIG_FOLDER}/settings/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(${CMAKE_CONFIG_FOLDER}/settings/Doxygen.cmake)
enable_doxygen()

# allow for static analysis
include(${CMAKE_CONFIG_FOLDER}/settings/StaticAnalyzers.cmake)

if (STRATEGA_BUILD_DOCS)
    add_subdirectory("docs")
endif ()

include(${CMAKE_CONFIG_FOLDER}/settings/Conan.cmake)
run_conan()
include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
include(${PROJECT_BINARY_DIR}/conan_paths.cmake)
#
find_package(Threads)
find_package(yaml-cpp REQUIRED)
find_package(recastnavigation REQUIRED)
find_package(imgui REQUIRED)
if (CMAKE_SYSTEM_NAME MATCHES Linux)
    add_subdirectory(${DEPENDENCY_DIR}/sfml)
else ()
    find_package(SFML COMPONENTS system window graphics REQUIRED)
endif ()

# Optional
if (STRATEGA_BUILD_BINDINGS)
    include(${CMAKE_CONFIG_FOLDER}/TargetsBindings.cmake)
endif ()

if (STRATEGA_BUILD_DOCS)
    add_subdirectory("docs")
endif ()


# define the interface targets and optional tests
include(${CMAKE_CONFIG_FOLDER}/TargetsStratega.cmake)
if (ENABLE_TESTING)
    enable_testing()
    message(
            "Tests enabled, configuring."
    )
    include(${CMAKE_CONFIG_FOLDER}/TargetsTest.cmake)
endif ()

