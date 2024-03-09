#include "helper.hpp"


template <typename Maker>
static void BM_AlignMemory(benchmark::State& state, Maker maker) {
    for (auto _ : state) {
        runBenchBatch(state, maker, "test", 152, "longlonglongstring", -999, "rwars");
    }
}

using Aggr = MakeAggregator<"Test-{}{} long static string{} {}{}"_str,
        RuntimeStr<5>, RuntimeIntegral<int, 5>, RuntimeStr<20>, QuotedStr<RuntimeIntegral<int, 6>>, RuntimeStr<8>>;

static inline Maker<Aggr, LocalStorage, DefaultConfig> unaligned;
static inline Maker<Aggr, details::LocalStorageAlign, DefaultConfig> aligned;

static inline Maker<Aggr, LocalStorage, DynamicConfig> unalignedDyn;
static inline Maker<Aggr, details::LocalStorageAlign, DynamicConfig> alignedDyn;


BENCHMARK_CAPTURE(BM_AlignMemory, UnalignedDefault, unaligned);
BENCHMARK_CAPTURE(BM_AlignMemory, AlignedDefault, aligned);
BENCHMARK_CAPTURE(BM_AlignMemory, UnalignedDynamic, unalignedDyn);
BENCHMARK_CAPTURE(BM_AlignMemory, AlignedDynamic, alignedDyn);