#include "helper.hpp"

#include <chrono>

template <typename Maker>
static void BM_BNNMaker(benchmark::State& state, Maker maker) {

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    testMaker(state, maker, "BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", now.count(), "IOC");

    for (auto _ : state) {
        runBenchBatch(state, maker, "BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", now.count(), "IOC");
    }
}


static void BM_BNNFixedPatternMaker(benchmark::State& state) {

    using Aggr = Aggregator<StaticStr<"symbol="_str>, RuntimeStr<8>,
            DynamicLocated<StaticStr<"&side="_str>>, DynamicLocated<RuntimeStr<4>>,
            DynamicLocated<StaticStr<"&type="_str>>, DynamicLocated<RuntimeStr<5>>,
            DynamicLocated<StaticStr<"&quantity="_str>>, DynamicLocated<RuntimeFloat<>>,
            DynamicLocated<StaticStr<"&price="_str>>, DynamicLocated<RuntimeFloat<>>,
            DynamicLocated<StaticStr<"&newClientOrderId="_str>>, DynamicLocated<RuntimeStr<17>>,
            DynamicLocated<StaticStr<"&timestamp="_str>>, DynamicLocated<RuntimeIntegral<uint64_t, 16>>,
            DynamicLocated<StaticStr<"&newOrderRespType=RESULT&timeInForce="_str>>, DynamicLocated<RuntimeStr<3>>>;
    auto maker = Maker<Aggr>();

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    testMaker(state, maker, "BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", now.count(), "IOC");

    for (auto _ : state) {
        runBenchBatch(state, maker, "BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", now.count(), "IOC");
    }
}



static void BM_BNNFormat(benchmark::State& state) {

    std::string_view sv;
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {
            sv = std::format(
                    "symbol={}&side={}&type={}&quantity={}&price={}&newClientOrderId={}&timestamp={}&newOrderRespType=RESULT&timeInForce={}"
                    , "BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", now.count(), "IOC");
            benchmark::DoNotOptimize(sv);
        }
    }
}


template <typename Maker>
static void BM_OKXMaker(benchmark::State& state, Maker maker) {

    testMaker(state, maker, "USDT", "orderiduniquestr", 1.567, "buy", 44865.2, "BTC-USDT", "limit", "cross");

    for (auto _ : state) {
        runBenchBatch(state, maker, "USDT", "orderiduniquestr", 1.567, "buy", 44865.2, "BTC-USDT", "limit", "cross");
    }
}


static void BM_OKXFormat(benchmark::State& state) {

    std::string sv;
    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {
            sv = std::format(
                    R"({{"ccy":"{}","clOrdId":"{}","sz":"{}","side":"{}","px":"{}","instId":"{}","ordType":"{}","tdMode":"{}"}})"
                    , "USDT", "orderiduniquestr", 1.567, "buy", 44865.2, "BTC-USDT", "limit", "cross");
            benchmark::DoNotOptimize(sv);
        }
    }
}


using OKXAggr = Aggregator<StaticStr<R"({"ccy":)"_str>, QuotedStr<RuntimeStr<5>>,
        StaticStr<",\"clOrdId\":"_str>, QuotedStr<RuntimeStr<17>>,
        StaticStr<",\"sz\":"_str>, QuotedStr<RuntimeFloat<double>>,
        StaticStr<",\"side\":"_str>, QuotedStr<RuntimeStr<4>>,
        StaticStr<",\"px\":"_str>, QuotedStr<RuntimeFloat<double>>,
        StaticStr<",\"instId\":"_str>, QuotedStr<RuntimeStr<12>>,
        StaticStr<",\"ordType\":"_str>, QuotedStr<RuntimeStr<9>>,
        StaticStr<",\"tdMode\":"_str>, QuotedStr<RuntimeStr<5>>,
        StaticStr<"}"_str>>;

using BNNAggr = Aggregator<StaticStr<"symbol="_str>, RuntimeStr<8>,
        StaticStr<"&side="_str>, RuntimeStr<4>,
        StaticStr<"&type="_str>, RuntimeStr<5>,
        StaticStr<"&quantity="_str>, RuntimeFloat<>,
        StaticStr<"&price="_str>, RuntimeFloat<>,
        StaticStr<"&newClientOrderId="_str>, RuntimeStr<17>,
        StaticStr<"&timestamp="_str>, RuntimeIntegral<uint64_t, 16>,
        StaticStr<"&newOrderRespType=RESULT&timeInForce="_str>, RuntimeStr<3>>;

BENCHMARK_CAPTURE(BM_BNNMaker, Default, Maker<BNNAggr, LocalStorage, DebugConfig<>>());
BENCHMARK_CAPTURE(BM_BNNMaker, Dynamic, Maker<BNNAggr, LocalStorage, DynamicConfig>());
BENCHMARK_CAPTURE(BM_BNNMaker, PreInit, Maker<BNNAggr, LocalStorage, PreInitConfig>());

BENCHMARK_CAPTURE(BM_BNNMaker, NoCacheDefault, Maker<BNNAggr>());
BENCHMARK_CAPTURE(BM_BNNMaker, NoCacheDynamic, Maker<BNNAggr, RingStorage::type, DynamicConfig>());
BENCHMARK_CAPTURE(BM_BNNMaker, NoCachePreInit, Maker<BNNAggr, RingStorage::type, PreInitConfig>());
BENCHMARK_CAPTURE(BM_BNNMaker, NoCachePreInitOnly, Maker<BNNAggr, RingStorage::type, PreInitOnlyConfig>());

BENCHMARK(BM_BNNFixedPatternMaker);
BENCHMARK(BM_BNNFormat)->Name("BM_BNN/Format");


BENCHMARK_CAPTURE(BM_OKXMaker, Default, Maker<OKXAggr>());
BENCHMARK_CAPTURE(BM_OKXMaker, Dynamic, Maker<OKXAggr, LocalStorage, DynamicConfig>());
BENCHMARK_CAPTURE(BM_OKXMaker, PreInit, Maker<OKXAggr, LocalStorage, PreInitConfig>());

BENCHMARK_CAPTURE(BM_OKXMaker, NoCacheDefault, Maker<OKXAggr>());
BENCHMARK_CAPTURE(BM_OKXMaker, NoCacheDynamic, Maker<OKXAggr, RingStorage::type, DynamicConfig>());
BENCHMARK_CAPTURE(BM_OKXMaker, NoCachePreInit, Maker<OKXAggr, RingStorage::type, PreInitConfig>());
BENCHMARK_CAPTURE(BM_OKXMaker, NoCachePreInitOnly, Maker<OKXAggr, RingStorage::type, PreInitOnlyConfig>());

BENCHMARK(BM_OKXFormat)->Name("BM_OKX/Format");