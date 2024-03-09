#include "helper.hpp"

template <typename Maker>
static void BM_DynamicLocatedOpt(benchmark::State& state, Maker maker, std::string const& str) {
    for (auto _ : state) {
        runBenchBatch(state, maker, str);
    }
}

using AggrLocatedEtalon = Aggregator<
        StaticStr<"test="_str>, RuntimeStr<50>,
        StaticStr<";"_str>>;

using AggrLongTail = Aggregator<
        StaticStr<"test="_str>, RuntimeStr<50>,
        StaticStr<"verylongtailverylongtailvery"_str>>;

static inline std::string SHORT_STR = "test"; // 4
static inline std::string LONG_STR = "testtesttesttesttesttesttesttesttesttesttest"; // 44
static inline std::string FULL_50_STR = "testtesttesttesttesttesttesttesttesttesttestTESTte"; // 50


#define BENCH_INIT(Name, Aggr) \
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## ShortLocalDefault, Maker<Aggr, LocalStorage, DefaultConfig>(), SHORT_STR);\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## LongLocalDefault, Maker<Aggr, LocalStorage, DefaultConfig>(), LONG_STR);\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## FullLocalDefault, Maker<Aggr, LocalStorage, DefaultConfig>(), FULL_50_STR);\
\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## ShortRingDefault, Maker<Aggr, RingStorage::type, DefaultConfig>(), SHORT_STR);\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## LongRingDefault, Maker<Aggr, RingStorage::type, DefaultConfig>(), LONG_STR);\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## FullRingDefault, Maker<Aggr, RingStorage::type, DefaultConfig>(), FULL_50_STR);\
\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## ShortRingDynamic, Maker<Aggr, RingStorage::type, DynamicConfig>(), SHORT_STR);\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## LongRingDynamic, Maker<Aggr, RingStorage::type, DynamicConfig>(), LONG_STR);\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## FullRingDynamic, Maker<Aggr, RingStorage::type, DynamicConfig>(), FULL_50_STR);\
\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## ShortRingPreInitT, Maker<Aggr, RingStorage::type, PreInitConfig>(), SHORT_STR);\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## LongRingPreInitT, Maker<Aggr, RingStorage::type, PreInitConfig>(), LONG_STR);\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## FullRingPreInitT, Maker<Aggr, RingStorage::type, PreInitConfig>(), FULL_50_STR);\
\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## ShortRingPreInitOnly, Maker<Aggr, RingStorage::type, PreInitOnlyConfig>(), SHORT_STR);\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## LongRingPreInitOnly, Maker<Aggr, RingStorage::type, PreInitOnlyConfig>(), LONG_STR);\
BENCHMARK_CAPTURE(BM_DynamicLocatedOpt, Name ## FullRingPreInitOnly, Maker<Aggr, RingStorage::type, PreInitOnlyConfig>(), FULL_50_STR);


BENCH_INIT(Etalon, AggrLocatedEtalon);
BENCH_INIT(LongTail, AggrLongTail);

#undef BENCH_INIT