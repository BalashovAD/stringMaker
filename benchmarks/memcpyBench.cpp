#include "helper.hpp"

static inline constexpr auto TEST_STRING = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"_str;

static constexpr auto MEM_RUNS = 10'000;

template <size_t step>
static void BM_memcpyStd(benchmark::State& state, std::integral_constant<size_t, step>) {
    char dst[step * MEM_RUNS];
    for (auto _ : state) {
        for (auto i = 0; i != 10000; ++i) {
            memcpy(dst + i * step, TEST_STRING.c_str(), step);
        }
        benchmark::DoNotOptimize(dst);
    }

    if (memcmp(dst, TEST_STRING.c_str(), step) != 0) {
        state.SkipWithError("Strings are not equal");
    }
}


template <size_t N>
void fill(char* ptr, ConstexprString<N> const& str) noexcept {
    if constexpr (N < 8) {
        if constexpr (N > 0) ptr[0] = str[0];
        if constexpr (N > 1) ptr[1] = str[1];
        if constexpr (N > 2) ptr[2] = str[2];
        if constexpr (N > 3) ptr[3] = str[3];
        if constexpr (N > 4) ptr[4] = str[4];
        if constexpr (N > 5) ptr[5] = str[5];
        if constexpr (N > 6) ptr[6] = str[6];
    } else {
        memcpy(ptr, str.c_str(), N);
    }
}

template <size_t step>
static void BM_memcpyConstexpr(benchmark::State& state, std::integral_constant<size_t, step>) {
    constexpr auto cfg = DefaultConfig();

    char dst[step * MEM_RUNS];
    for (auto _ : state) {
        for (auto i = 0; i != 10000; ++i) {
            cfg.fill(dst + i * step, TEST_STRING.substr<0, step>());
        }
        benchmark::DoNotOptimize(dst);
    }

    if (memcmp(dst, TEST_STRING.c_str(), step) != 0) {
        state.SkipWithError("Strings are not equal");
    }
}

#define CONST_WRAPPER(x) std::integral_constant<size_t, 1>{}

BENCHMARK_CAPTURE(BM_memcpyStd, T1, CONST_WRAPPER(1));
BENCHMARK_CAPTURE(BM_memcpyConstexpr, T1, CONST_WRAPPER(1));

BENCHMARK_CAPTURE(BM_memcpyStd, T5, CONST_WRAPPER(5));
BENCHMARK_CAPTURE(BM_memcpyConstexpr, T5, CONST_WRAPPER(5));

BENCHMARK_CAPTURE(BM_memcpyStd, T7, CONST_WRAPPER(7));
BENCHMARK_CAPTURE(BM_memcpyConstexpr, T7, CONST_WRAPPER(7));


BENCHMARK_CAPTURE(BM_memcpyStd, T8, CONST_WRAPPER(8));
BENCHMARK_CAPTURE(BM_memcpyConstexpr, T8, CONST_WRAPPER(8));

#undef CONST_WRAPPER
