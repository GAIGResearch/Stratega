# Include sub-projects.
#add_subdirectory("GUI")
#add_subdirectory("Arena")


cmake_minimum_required(VERSION 3.15)

project(stratega LANGUAGES CXX)

if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif ()

set(CMAKE_CONFIG_FOLDER "cmake")

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

message("C++ Compiler: ${CMAKE_CXX_COMPILER}")
message("C++ Compiler Info: ${CMAKE_CXX_COMPILER_ID}, version ${CMAKE_CXX_COMPILER_VERSION}")
message("CURRENT CMAKE BINARY DIR: ${CMAKE_CURRENT_BINARY_DIR}")

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
    if (ENABLE_BUILD_WITH_TIME_TRACE)
        add_compile_definitions(project_options INTERFACE -ftime-trace)
    endif ()
endif ()

set(BUILD_SHARED_LIBS FALSE)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
set(project_cxx_standard cxx_std_20)
set(project_CPP_STANDARD C++20)
target_compile_features(project_options INTERFACE ${project_cxx_standard})
set_property(TARGET project_options PROPERTY CXX_STANDARD ${project_CPP_STANDARD})
# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

#enable clang-format and clang-tidy project wide
include(${CMAKE_CONFIG_FOLDER}/Clang-cxx-dev-tools.cmake)

# enable cache system
include(${CMAKE_CONFIG_FOLDER}/Cache.cmake)

# standard compiler warnings
include(${CMAKE_CONFIG_FOLDER}/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(${CMAKE_CONFIG_FOLDER}/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(${CMAKE_CONFIG_FOLDER}/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(${CMAKE_CONFIG_FOLDER}/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_TESTING "Enable Test Builds" ON)
option(ENABLE_FUZZING "Enable Fuzzing Builds" OFF)

option(STRATEGA_BUILD_DOCS "Enable to build the docs. Requires doxygen to be installed on the system" OFF)
option(STRATEGA_BUILD_BINDINGS "Enable to build the bindings to other languages." OFF)

# Very basic PCH example
option(ENABLE_PCH "Enable Precompiled Headers" ON)
if (ENABLE_PCH)
    # This sets a global PCH parameter, each project will build its own PCH, which
    # is a good idea if any #define's change
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

if (STRATEGA_BUILD_DOCS)
    add_subdirectory("docs")
endif ()

set(DEPENDENCY_DIR deps)
if (CMAKE_SYSTEM_NAME MATCHES Linux)
    set(CONANFILE conanfile_linux.txt)
#    set(SFML_STATIC_LIBRARIES TRUE)
    include(${CMAKE_CONFIG_FOLDER}/FetchSFML.cmake)
    include(${CMAKE_CONFIG_FOLDER}/FindSFML.cmake)
else ()
    set(CONANFILE conanfile.txt)
endif ()

include(${CMAKE_CONFIG_FOLDER}/Conan.cmake)
run_conan()
include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
include(${PROJECT_BINARY_DIR}/conan_paths.cmake)

find_package(Threads)
find_package(yaml-cpp REQUIRED)
find_package(recastnavigation REQUIRED)
find_package(imgui REQUIRED)
find_package(SFML COMPONENTS system window graphics PATHS ${sfml_BINARY_DIR} REQUIRED)


# TODO: Check if this is really necessary to do?
#set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "disable yaml tests")
#set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "disable yaml tools")
#set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "disable yaml contrib")
#set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "disable shared libs")
#add_subdirectory(${yaml_cpp_SOURCE_DIR} ${yaml_cpp_BINARY_DIR} EXCLUDE_FROM_ALL)

set(STRATEGA_BINDINGS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bindings)
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

