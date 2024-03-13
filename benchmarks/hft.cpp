#include "helper.hpp"

#include <chrono>

template <typename Maker>
static void BM_BNN(benchmark::State& state, Maker maker) {

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    testMaker(state, maker, "BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", now.count(), "IOC");

    for (auto _ : state) {
        runBenchBatch(state, maker, "BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", now.count(), "IOC");
    }
    state.SetItemsProcessed(TEST_SIZE * state.iterations());
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
    state.SetItemsProcessed(TEST_SIZE * state.iterations());
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
    state.SetItemsProcessed(TEST_SIZE * state.iterations());
}



static void BM_BNNStream(benchmark::State& state) {

    std::string sv;
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {
            std::ostringstream oss;
            oss << "symbol=" << "BTCUSDT"
                << "&side=" << "BUY"
                << "&type=" << "LIMIT"
                << "&quantity=" << 1.568
                << "&price=" << 44200.5
                << "&newClientOrderId=" << "cuniquestring"
                << "&timestamp=" << now.count()
                << "&newOrderRespType=RESULT"
                << "&timeInForce=" << "IOC";

            sv = oss.str();
            benchmark::DoNotOptimize(sv);
        }
    }
    state.SetItemsProcessed(TEST_SIZE * state.iterations());
}

static void BM_BNNPerfect(benchmark::State& state) {
    DefaultConfig cfg{};
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {

            char buffer[250];
            const auto end = buffer + sizeof(buffer);
            auto* ptr = buffer;

            auto const addStrLiteral = [&](const char* str) {
                auto len = std::strlen(str);
                cfg.memcpy(ptr, str, len);
                ptr += len;
            };

            auto const addNumber = [&](auto const& t) {
                ptr = std::to_chars(ptr, end, t).ptr;
            };

            addStrLiteral("symbol=");
            addStrLiteral("BTCUSDT");
            addStrLiteral("&");
            addStrLiteral("side=");
            addStrLiteral("BUY");
            addStrLiteral("&");
            addStrLiteral("type=");
            addStrLiteral("LIMIT");
            addStrLiteral("&");
            addStrLiteral("quantity=");
            addNumber(1.568);
            addStrLiteral("&");
            addStrLiteral("price=");
            addNumber(44200.5);
            addStrLiteral("&");
            addStrLiteral("newClientOrderId=");
            addStrLiteral("cuniquestring");
            addStrLiteral("&");
            addStrLiteral("timestamp=");
            addNumber(now.count());
            addStrLiteral("&");
            addStrLiteral("newOrderRespType=RESULT&");
            addStrLiteral("timeInForce=");
            addStrLiteral("IOC");

            benchmark::DoNotOptimize(buffer);
        }
    }

    state.SetItemsProcessed(TEST_SIZE * state.iterations());
}


template <typename Maker>
static void BM_OKX(benchmark::State& state, Maker maker) {

    testMaker(state, maker, "USDT", "orderiduniquestr", 1.567, "buy", 44865.2, "BTC-USDT", "limit", "cross");

    for (auto _ : state) {
        runBenchBatch(state, maker, "USDT", "orderiduniquestr", 1.567, "buy", 44865.2, "BTC-USDT", "limit", "cross");
    }

    state.SetItemsProcessed(TEST_SIZE * state.iterations());
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

    state.SetItemsProcessed(TEST_SIZE * state.iterations());
}

static void BM_OKXStream(benchmark::State& state) {

    std::string sv;
    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {
            std::ostringstream oss;
            oss << R"({"ccy":")" << "USDT"
                << R"(","clOrdId":")" << "orderiduniquestr"
                << R"(","sz":")" << 1.567
                << R"(","side":")" << "buy"
                << R"(","px":")" << 44865.2
                << R"(","instId":")" << "BTC-USDT"
                << R"(","ordType":")" << "limit"
                << R"(","tdMode":")" << "cross" << R"("})";
            sv = oss.str();
            benchmark::DoNotOptimize(sv);
        }
    }

    state.SetItemsProcessed(TEST_SIZE * state.iterations());
}

static void BM_OKXPerfect(benchmark::State& state) {

    DefaultConfig cfg{};

    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {

            char buffer[1024];
            const auto end = buffer + sizeof(buffer);
            auto* ptr = buffer;

            auto const addStrLiteral = [&](const char* str) {
                auto len = std::strlen(str);
                cfg.memcpy(ptr, str, len);
                ptr += len;
            };

            auto const addNumber = [&](auto const& t) {
                ptr = std::to_chars(ptr, end, t).ptr;
            };

            addStrLiteral(R"({"ccy":")");
            addStrLiteral("USDT");
            addStrLiteral(R"(","clOrdId":")");
            addStrLiteral("orderiduniquestr");
            addStrLiteral(R"(","sz":")");
            addNumber(1.567);
            addStrLiteral(R"(","side":")");
            addStrLiteral("buy");
            addStrLiteral(R"(","px":")");
            addNumber(44865.2);
            addStrLiteral(R"(","instId":")");
            addStrLiteral("BTC-USDT");
            addStrLiteral(R"(","ordType":")");
            addStrLiteral("limit");
            addStrLiteral(R"(","tdMode":")");
            addStrLiteral("cross");
            addStrLiteral(R"("})");

            benchmark::DoNotOptimize(buffer);
        }
    }

    state.SetItemsProcessed(TEST_SIZE * state.iterations());
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

BENCHMARK_CAPTURE(BM_BNN, Default, Maker<BNNAggr>());
BENCHMARK_CAPTURE(BM_BNN, Dynamic, Maker<BNNAggr, LocalStorage, DynamicConfig>());
BENCHMARK_CAPTURE(BM_BNN, PreInit, Maker<BNNAggr, LocalStorage, PreInitConfig>());

BENCHMARK_CAPTURE(BM_BNN, RingDefault, Maker<BNNAggr, RingStorage::type>());
BENCHMARK_CAPTURE(BM_BNN, RingDynamic, Maker<BNNAggr, RingStorage::type, DynamicConfig>());
BENCHMARK_CAPTURE(BM_BNN, RingPreInit, Maker<BNNAggr, RingStorage::type, PreInitConfig>());
BENCHMARK_CAPTURE(BM_BNN, RingPreInitOnly, Maker<BNNAggr, RingStorage::type, PreInitOnlyConfig>());

BENCHMARK(BM_BNNFixedPatternMaker)->Name("BM_BNN/FixedPatternMaker");
BENCHMARK(BM_BNNFormat)->Name("BM_BNN/Format");
BENCHMARK(BM_BNNStream)->Name("BM_BNN/Stream");
BENCHMARK(BM_BNNPerfect)->Name("BM_BNN/Perfect");

BENCHMARK_CAPTURE(BM_OKX, Default, Maker<OKXAggr>());
BENCHMARK_CAPTURE(BM_OKX, Dynamic, Maker<OKXAggr, LocalStorage, DynamicConfig>());
BENCHMARK_CAPTURE(BM_OKX, PreInit, Maker<OKXAggr, LocalStorage, PreInitConfig>());

BENCHMARK_CAPTURE(BM_OKX, RingDefault, Maker<OKXAggr>());
BENCHMARK_CAPTURE(BM_OKX, RingDynamic, Maker<OKXAggr, RingStorage::type, DynamicConfig>());
BENCHMARK_CAPTURE(BM_OKX, RingPreInit, Maker<OKXAggr, RingStorage::type, PreInitConfig>());
BENCHMARK_CAPTURE(BM_OKX, RingPreInitOnly, Maker<OKXAggr, RingStorage::type, PreInitOnlyConfig>());

BENCHMARK(BM_OKXFormat)->Name("BM_OKX/Format");
BENCHMARK(BM_OKXStream)->Name("BM_OKX/Stream");
BENCHMARK(BM_OKXPerfect)->Name("BM_OKX/Perfect");