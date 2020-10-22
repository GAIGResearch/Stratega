cmake_minimum_required(VERSION 3.0)

project(RecastNavigation)

# lib versions
SET(SOVERSION 1)
SET(VERSION 1.0.0)

option(RECASTNAVIGATION_DEMO "Build demo" OFF)
option(RECASTNAVIGATION_TESTS "Build tests" OFF)
option(RECASTNAVIGATION_EXAMPLES "Build examples" OFF)
option(RECASTNAVIGATION_STATIC "Build static libraries" ON)

include(GNUInstallDirs)

add_subdirectory(DebugUtils)
add_subdirectory(Detour)
add_subdirectory(DetourCrowd)
add_subdirectory(DetourTileCache)
add_subdirectory(Recast)

if (RECASTNAVIGATION_DEMO)
    add_subdirectory(RecastDemo)
endif ()

if (RECASTNAVIGATION_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif ()

