cmake_minimum_required(VERSION 3.24)
project(strMaker)

set(CMAKE_CXX_STANDARD 20)

option(ENABLE_HUGEPAGES "Huge pages" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wno-error=old-style-cast -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math")

file(GLOB LIB_SOURCES
        "./include/*.cpp"
        "./include/*.h"
        "./include/*.hpp"
        "./include/*.cxx")

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/extern)

add_subdirectory(tests)
add_subdirectory(benchmarks)
