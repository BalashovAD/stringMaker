#include "helper.hpp"

template <typename Maker>
static void BM_staticZeroCost(benchmark::State& state, Maker& maker) {
    for (auto _ : state) {
        runBenchBatch(state, maker, "testTEST", 1234);
    }
}

static void BM_staticZeroCostFormat(benchmark::State& state) {
    for (auto _ : state) {
        for (auto i = 0u; i != TEST_SIZE; ++i) {
            auto sv = std::format("test{}ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss{}", "testTEST", 1234);
            benchmark::DoNotOptimize(sv);
        }
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
        RuntimeIntegral<unsigned>>; // 60 's'

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
        runBenchBatch(state, maker, "test");
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


inline static auto aLotOfQuoted3 = Maker<
        Aggregator<
                StaticStr<"start_"_str>, QuotedStr<QuotedStr<QuotedStr<RuntimeStr<10>, '@'>, '@'>, '@'>,
                StaticStr<"_end"_str>>>();

inline static auto aLotOfQuoted4 = Maker<
        Aggregator<
                StaticStr<"start_"_str>, QuotedStr<QuotedStr<QuotedStr<QuotedStr<RuntimeStr<10>, '@'>, '@'>, '@'>, '@'>,
                StaticStr<"_end"_str>>>();

BENCHMARK_CAPTURE(BM_Quoted, noQuoted, noQuoted);
BENCHMARK_CAPTURE(BM_Quoted, quoted, quoted);
BENCHMARK_CAPTURE(BM_Quoted, aLotOfQuoted3, aLotOfQuoted3);
BENCHMARK_CAPTURE(BM_Quoted, aLotOfQuoted4, aLotOfQuoted4);
