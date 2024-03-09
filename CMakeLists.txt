cmake_minimum_required(VERSION 3.24)
project(strMaker)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wno-error=old-style-cast -Wall")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unknown-warning-option")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

file(GLOB LIB_SOURCES
        "./include/*.cpp"
        "./include/*.h"
        "./include/*.hpp"
        "./include/*.cxx")

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/extern)

add_subdirectory(tests)
add_subdirectory(benchmarks)
