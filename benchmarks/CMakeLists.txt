# set THIRD_PARTY_DIR

set(BENCHMARK_ENABLE_TESTING off)

set(BENCHMARK_DIR ${THIRD_PARTY_DIR}/benchmark)
add_subdirectory(${BENCHMARK_DIR} ${CMAKE_BINARY_DIR}/benchmark)
set(BENCHMARK_LIBRARIES benchmark::benchmark)

file(GLOB_RECURSE BENCHMARK_SOURCES
        "./*.cpp"
        "./*.h"
        "./*.hpp"
        "./*.cxx")

add_executable(bench ${BENCHMARK_SOURCES} ${LIB_SOURCES})

target_include_directories(bench PRIVATE
        ${BENCHMARK_DIR}/include)
target_link_libraries(bench ${BENCHMARK_LIBRARIES})

if(${ENABLE_HUGEPAGES})
    target_link_options(bench PRIVATE "-B/hugetlbfs")
endif ()