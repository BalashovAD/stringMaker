# set THIRD_PARTY_DIR

enable_testing()
set(GTEST_DIR ${THIRD_PARTY_DIR}/googletest)
add_subdirectory(${GTEST_DIR} ${CMAKE_BINARY_DIR}/googletest)

file(GLOB_RECURSE TEST_SOURCES
        "./*.cpp"
        "./*.h"
        "./*.hpp"
        "./*.cxx")

add_executable(unit-tests ${TEST_SOURCES} ${LIB_SOURCES})
target_link_libraries(unit-tests PRIVATE gtest gtest_main)

#target_include_directories(unit-tests PRIVATE ${CMAKE_SOURCE_DIR}/single_include)

target_include_directories(unit-tests PRIVATE
        ${GTEST_DIR}/googletest/include)

add_test(NAME unit-tests
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)