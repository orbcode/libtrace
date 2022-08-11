cmake_minimum_required(VERSION 3.18)

project(OrbcodeLibtrace NONE)

option(ORBCODE_LIBTRACE_BUILD_TEST "Build tests" OFF)
option(ORBCODE_LIBTRACE_DOCS "Build Doxygen documentation" OFF)

if(ORBCODE_LIBTRACE_BUILD_TEST)
    enable_language(C)
    enable_language(CXX)
endif()

add_subdirectory(libs)

if(ORBCODE_LIBTRACE_BUILD_TEST)
    add_subdirectory(tests)
endif()

if(ORBCODE_LIBTRACE_DOCS)
    add_subdirectory(docs)
endif()