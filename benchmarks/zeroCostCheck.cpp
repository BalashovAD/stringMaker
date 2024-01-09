#include <benchmark/benchmark.h>

#include <format>

#include "../include/strMaker/full.hpp"

using namespace mkr;

static constexpr int TEST_SIZE = 1000;
using RingStorage = RingBufferStorageWrapper<TEST_SIZE, false>;

template <typename Maker>
static void BM_staticZeroCost(benchmark::State& state, Maker& maker) {
    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {
            auto sv = maker.generate("testTEST", 1234);
            benchmark::DoNotOptimize(sv);
        }
    }
}

static void BM_staticZeroCostFormat(benchmark::State& state) {
    for (auto _ : state) {
        auto sv = std::format("test{}ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss{}", "testTEST", 1234);
        benchmark::DoNotOptimize(sv);
    }
}

using AggrEtalon = Aggregator<
        StaticStr<"test"_str>, RuntimeStr<10>,
        StaticStr<"ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss"_str>,
        RuntimeIntegral<unsigned>>;

inline static auto staticCheckEtalon = Maker<AggrEtalon, RingStorage::type>();
inline static auto staticCheckEtalonDynamic = Maker<AggrEtalon, RingStorage::type, DynamicConfig>();

using AlotOfS = Aggregator<
        StaticStr<"test"_str>, RuntimeStr<10>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,StaticStr<"s"_str>,
        RuntimeIntegral<unsigned>>;

inline static auto staticCheckMultiStaticStr = Maker<AlotOfS, RingStorage::type>();
inline static auto staticCheckMultiStaticStrDynamic = Maker<AlotOfS, RingStorage::type, DynamicConfig>();

using AggrMisSize = Aggregator<
        StaticStr<"test"_str>, RuntimeStr<50>,
        StaticStr<"ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss"_str>,
        RuntimeIntegral<unsigned>>;

inline static auto misSize = Maker<AggrMisSize, RingStorage::type>();

BENCHMARK(BM_staticZeroCostFormat);
BENCHMARK_CAPTURE(BM_staticZeroCost, NoCacheEtalon, staticCheckEtalon);
BENCHMARK_CAPTURE(BM_staticZeroCost, NoCacheMultiStr, staticCheckMultiStaticStr);
BENCHMARK_CAPTURE(BM_staticZeroCost, NoCacheEtalonDynamic, staticCheckEtalonDynamic);
BENCHMARK_CAPTURE(BM_staticZeroCost, NoCacheMultiStrDynamic, staticCheckMultiStaticStrDynamic);
BENCHMARK_CAPTURE(BM_staticZeroCost, misSize, misSize);

template <typename Maker>
static void BM_Quoted(benchmark::State& state, Maker& maker) {
    for (auto _ : state) {
        auto sv = maker.generate("test");
        benchmark::DoNotOptimize(sv);
    }
}

inline static auto noQuoted = Maker<
        Aggregator<
                StaticStr<"start_@"_str>, RuntimeStr<10>,
                StaticStr<"@_end"_str>>>();

inline static auto quoted = Maker<
        Aggregator<
                StaticStr<"start_"_str>, QuotedStr<RuntimeStr<10>, '@'>,
                StaticStr<"_end"_str>>>();


inline static auto aLotOfQuoted = Maker<
        Aggregator<
                StaticStr<"start_"_str>, QuotedStr<QuotedStr<QuotedStr<QuotedStr<RuntimeStr<10>, '@'>, '@'>, '@'>, '@'>,
                StaticStr<"_end"_str>>>();

BENCHMARK_CAPTURE(BM_Quoted, noQuoted, noQuoted);
BENCHMARK_CAPTURE(BM_Quoted, quoted, quoted);
BENCHMARK_CAPTURE(BM_Quoted, aLotOfQuoted, aLotOfQuoted);
