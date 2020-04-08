cmake_minimum_required(VERSION 3.15)
project(persistence_manager)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there. Thank you.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 17)

include(build/BuildDefaults.cmake)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE sqlite3 REQUIRED)

if(BUILD_TESTING)
    include(FetchContentGoogleTest)

    enable_testing()
    add_subdirectory("test")
endif()
