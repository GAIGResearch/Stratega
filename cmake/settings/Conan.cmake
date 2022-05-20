macro(run_conan)
    # Download automatically
    if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(
                STATUS
                "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/0.17.0/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake")
    endif ()

    include(${CMAKE_BINARY_DIR}/conan.cmake)
    
    conan_add_remote(NAME bincrafters URL
            https://api.bintray.com/conan/bincrafters/public-conan)

    conan_cmake_run(
            #Uncomment if building for M1 Apple
            #ARCH armv8
            CONANFILE ${DEPENDENCY_DIR}/${CONANFILE}
            CONAN_COMMAND ${CONAN_PATH}
            ${CONAN_EXTRA_REQUIRES}
            OPTIONS
            ${CONAN_EXTRA_OPTIONS}
            BASIC_SETUP NO_OUTPUT_DIRS
            CMAKE_TARGETS # individual targets to link to
            BUILD missing
    )
endmacro()
