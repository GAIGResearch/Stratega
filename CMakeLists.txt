cmake_minimum_required(VERSION 3.8)

project(Stratega LANGUAGES CXX)

# project set'tings
set(CMAKE_CONFIG_FOLDER "cmake")
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
if (CMAKE_SYSTEM_NAME MATCHES Linux)
    set(CONANFILE conanfile_linux.txt)
else ()
    set(CONANFILE conanfile.txt)
endif ()
set(DEPENDENCY_DIR "deps")
#set(PROJ_CXX_STD_FEATURE cxx_std_14)
#set(PROJ_CXX_STANDARD C++14)
set(PROJ_CXX_STD_FEATURE cxx_std_17)
set(PROJ_CXX_STANDARD C++17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 17)
set(CONAN_SYSTEM_INCLUDES ON)
set(CMAKE_NO_SYSTEM_FROM_IMPORTED OFF)
set(PROJ_SOURCES_DIR "${CMAKE_SOURCE_DIR}/src")
set(PROJ_TEST_FOLDER "${CMAKE_SOURCE_DIR}/test")
set(SUBPROJ_PYBINDING_DIR "${PROJ_SOURCES_DIR}/python")
set(SUBPROJ_PYBINDING_SRC_DIR "${SUBPROJ_PYBINDING_DIR}/src")
set(SUBPROJ_STRATEGA_DIR "${PROJ_SOURCES_DIR}/stratega")
set(SUBPROJ_STRATEGA_INCLUDE_DIR "${SUBPROJ_STRATEGA_DIR}/include")
set(SUBPROJ_STRATEGA_SRC_DIR "${SUBPROJ_STRATEGA_DIR}/src")
set(SUBPROJ_ARENA_DIR "${PROJ_SOURCES_DIR}/arena")
set(SUBPROJ_ARENA_SRC_DIR "${SUBPROJ_ARENA_DIR}/src")
set(SUBPROJ_GUI_DIR "${PROJ_SOURCES_DIR}/gui")
set(SUBPROJ_GUI_SRC_DIR "${SUBPROJ_GUI_DIR}/src")
set(SUBPROJ_NTBEA_DIR "${PROJ_SOURCES_DIR}/ntbea")
set(SUBPROJ_NTBEA_SRC_DIR "${SUBPROJ_NTBEA_DIR}/src")


# options
option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_BUILD_DOCS "Enable building the docs. Requires doxygen to be installed on the system" OFF)
option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)
option(ENABLE_IPO "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)" OFF)
option(ENABLE_PCH "Enable Precompiled Headers" OFF)
option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer" OFF)
option(ENABLE_STRATEGA_BINDINGS "Enable to build the bindings to other languages." OFF)
option(ENABLE_TESTING "Enable Test Builds" ON)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors (If ENABLE_STRATEGA_BINDINGS is enabled, this option will be disabled automatically)" OFF)

option(SGA_BUILD_HEADLESS "Compile Stratega in headless mode" OFF)

#If CMAKE_NO_SYSTEM_FROM_IMPORTED is OFF, pybind11 should be treated as SYSTEM and it should not throw warnings
#but it does for some reason that we dont know.
#The temporal solution for this problem is to disable the warnings when we want to build the pyhton bindings
if(ENABLE_STRATEGA_BINDINGS)
message("\nDeactivated warnings as errors.\n")
    set(WARNINGS_AS_ERRORS OFF)   
endif()

# print out a short summary of the compiler inforMoved Arena and GUI filesmation
message("C++ Compiler: ${CMAKE_CXX_COMPILER}")
message("C++ Compiler Info: ${CMAKE_CXX_COMPILER_ID}, version ${CMAKE_CXX_COMPILER_VERSION}")
message("C++ STD Version: ${PROJ_CXX_STD_FEATURE}")
message("Build type: ${CMAKE_BUILD_TYPE}")
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

target_compile_features(project_options INTERFACE ${PROJ_CXX_STD_FEATURE})

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

# configure conan and fetch all dependencies
if(NOT SGA_BUILD_HEADLESS)   
   if (CMAKE_SYSTEM_NAME MATCHES Linux)
    # on linux SFML is not (yet) available from conan due to a missing lib dependency in their recipe.
    # We will have to fetch it manually in this case and add it to our project.
    include(${CMAKE_CONFIG_FOLDER}/modules/FetchSFML.cmake)
    endif ()
    include(${CMAKE_CONFIG_FOLDER}/modules/FetchIMGUI-SFML.cmake)
else()
    add_compile_definitions(BUILD_HEADLESS)
    set(ENABLE_STRATEGA_BINDINGS OFF)
endif()

include(${CMAKE_CONFIG_FOLDER}/settings/Conan.cmake)
run_conan()
include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
include(${PROJECT_BINARY_DIR}/conan_paths.cmake)


# find the downloaded/installed packages
if (CMAKE_SYSTEM_NAME MATCHES Linux)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
 endif ()
find_package(Threads)

if(NOT SGA_BUILD_HEADLESS)
   find_package(imgui REQUIRED)
   find_package(OpenGL REQUIRED)
   if (CMAKE_SYSTEM_NAME MATCHES Linux)
       add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR} EXCLUDE_FROM_ALL)
   else ()
       find_package(SFML COMPONENTS system window graphics REQUIRED)
   endif ()
endif()

find_package(Boost REQUIRED COMPONENTS random)
find_package(yaml-cpp REQUIRED)
find_package(recastnavigation REQUIRED)


# define the targets of the project
if(NOT SGA_BUILD_HEADLESS)
   include(${CMAKE_CONFIG_FOLDER}/targets/IMGUI.cmake)
    include(${CMAKE_CONFIG_FOLDER}/targets/GUI.cmake)
endif()

include(${CMAKE_CONFIG_FOLDER}/targets/Arena.cmake)
include(${CMAKE_CONFIG_FOLDER}/targets/Stratega.cmake)
include(${CMAKE_CONFIG_FOLDER}/targets/NTBEA.cmake)
if (ENABLE_STRATEGA_BINDINGS)
    message("\nPyBindings enabled, configuring.\n")
    add_subdirectory("src/python")
endif ()
if (ENABLE_BUILD_DOCS)
    message("\nDocs enabled, configuring.\n")
    add_subdirectory("docs")
endif ()
if (ENABLE_TESTING)
    enable_testing()
    message("\nTests enabled, configuring.\n")
    include(${CMAKE_CONFIG_FOLDER}/targets/Tests.cmake)
endif ()
