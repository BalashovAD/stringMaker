#include "helper.hpp"

using RS = RuntimeStr<8>;

using MadeAggr = MakeAggregator<"Test{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}"_str,
        RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS>;
using MadeUnoptimized = details::Unoptimized<"Test{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}"_str,
        RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS>;
using AggrEtalon = Aggregator<StaticStr<"Test"_str>,
        RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS, RS>;

static auto makerEtalon = Maker<AggrEtalon>{};
static auto makerMade = Maker<MadeAggr>{};
static auto makerUnoptimized = Maker<MadeUnoptimized>{};

template <typename Maker>
static void BM_MakeAggr(benchmark::State& state, Maker& maker) {
    std::array<std::string, 15> ss;
    std::transform(ss.begin(), ss.end(), ss.begin(), [](auto) {return generateRandomString(getRandomNumber(1, 8));});

    testMaker(state, maker,
            ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7]
                  , ss[8], ss[9], ss[10], ss[11], ss[12], ss[13], ss[14]);

    for (auto _ : state) {
        runBenchBatch(state, maker,
                ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7]
                      , ss[8], ss[9], ss[10], ss[11], ss[12], ss[13], ss[14]);
    }
}

BENCHMARK_CAPTURE(BM_MakeAggr, Etalon, makerEtalon);
BENCHMARK_CAPTURE(BM_MakeAggr, Maker, makerMade);
BENCHMARK_CAPTURE(BM_MakeAggr, Unoptimized, makerUnoptimized);