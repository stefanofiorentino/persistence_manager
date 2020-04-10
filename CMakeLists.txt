cmake_minimum_required(VERSION 3.15)
project(persistence_manager)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there. Thank you.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 17)

include(cmake/BuildDefaults.cmake)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE sqlite3 REQUIRED)

add_library(${PROJECT_NAME}_a STATIC src/persistence_manager.cpp)
target_include_directories(${PROJECT_NAME}_a PRIVATE ${PROJECT_SOURCE_DIR})

if(BUILD_TESTING)
    fetch_content_googletest()

    enable_testing()
    add_subdirectory("test")
endif()
