#include <gtest/gtest.h>

#include <format>

#include "../../include/strMaker/full.hpp"

using namespace testing;
using namespace mkr;


TEST(BenchHFT, BNN) {
    // 161 symbols
    using Aggr = Aggregator<StaticStr<"symbol="_str>, RuntimeStr<8>,
        StaticStr<"&side="_str>, RuntimeStr<4>,
        StaticStr<"&type="_str>, RuntimeStr<5>,
        StaticStr<"&quantity="_str>, RuntimeFloat<>,
        StaticStr<"&price="_str>, RuntimeFloat<>,
        StaticStr<"&newClientOrderId="_str>, RuntimeStr<17>,
        StaticStr<"&timestamp="_str>, RuntimeIntegral<uint64_t, 16>,
        StaticStr<"&newOrderRespType=RESULT&timeInForce="_str>, RuntimeStr<3>>;

    auto maker = Maker<Aggr, LocalStorage, DebugConfig<>>();

    EXPECT_EQ(maker.generate("BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", 1703789943256630, "IOC"),
              "symbol=BTCUSDT &side=BUY &type=LIMIT&quantity=1.568             &price=44200.5           &newClientOrderId=cuniquestring    &timestamp=1703789943256630&newOrderRespType=RESULT&timeInForce=IOC");

    EXPECT_EQ(std::format("symbol={}&side={}&type={}&quantity={}&price={}&newClientOrderId={}&timestamp={}&newOrderRespType=RESULT&timeInForce={}",
            "BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", 1703789943256630, "IOC")
            , "symbol=BTCUSDT&side=BUY&type=LIMIT&quantity=1.568&price=44200.5&newClientOrderId=cuniquestring&timestamp=1703789943256630&newOrderRespType=RESULT&timeInForce=IOC");
}

TEST(BenchHFT, BNNFixed) {
    using Aggr = Aggregator<StaticStr<"symbol="_str>, RuntimeStr<8>,
        DynamicLocated<StaticStr<"&side="_str>>, DynamicLocated<RuntimeStr<4>>,
        DynamicLocated<StaticStr<"&type="_str>>, DynamicLocated<RuntimeStr<5>>,
        DynamicLocated<StaticStr<"&quantity="_str>>, DynamicLocated<RuntimeFloat<>>,
        DynamicLocated<StaticStr<"&price="_str>>, DynamicLocated<RuntimeFloat<>>,
        DynamicLocated<StaticStr<"&newClientOrderId="_str>>, DynamicLocated<RuntimeStr<17>>,
        DynamicLocated<StaticStr<"&timestamp="_str>>, DynamicLocated<RuntimeIntegral<uint64_t, 16>>,
        DynamicLocated<StaticStr<"&newOrderRespType=RESULT&timeInForce="_str>>, DynamicLocated<RuntimeStr<3>>>;

    auto maker = Maker<Aggr, LocalStorage, DebugConfig<>>();

    auto makerResult = maker.generate("BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", 1703789943256630, "IOC");

    EXPECT_EQ(makerResult, "symbol=BTCUSDT&side=BUY&type=LIMIT&quantity=1.568&price=44200.5&newClientOrderId=cuniquestring&timestamp=1703789943256630&newOrderRespType=RESULT&timeInForce=IOC");

    auto formatResult = std::format("symbol={}&side={}&type={}&quantity={}&price={}&newClientOrderId={}&timestamp={}&newOrderRespType=RESULT&timeInForce={}",
            "BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", 1703789943256630, "IOC");
    EXPECT_EQ(formatResult, "symbol=BTCUSDT&side=BUY&type=LIMIT&quantity=1.568&price=44200.5&newClientOrderId=cuniquestring&timestamp=1703789943256630&newOrderRespType=RESULT&timeInForce=IOC");
}


TEST(BenchHFT, BNNFixedMode) {
    using Aggr = Aggregator<StaticStr<"symbol="_str>, RuntimeStr<8>,
            StaticStr<"&side="_str>, RuntimeStr<4>,
            StaticStr<"&type="_str>, RuntimeStr<5>,
            StaticStr<"&quantity="_str>, RuntimeFloat<>,
            StaticStr<"&price="_str>, RuntimeFloat<>,
            StaticStr<"&newClientOrderId="_str>, RuntimeStr<17>,
            StaticStr<"&timestamp="_str>, RuntimeIntegral<uint64_t, 16>,
            StaticStr<"&newOrderRespType=RESULT&timeInForce="_str>, RuntimeStr<3>>;
    using Cfg = DynamicConfig;
    auto maker = Maker<Aggr, LocalStorage, Cfg>();

    auto makerResult = maker.generate("BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", 1703789943256630, "IOC");

    EXPECT_EQ(makerResult, "symbol=BTCUSDT&side=BUY&type=LIMIT&quantity=1.568&price=44200.5&newClientOrderId=cuniquestring&timestamp=1703789943256630&newOrderRespType=RESULT&timeInForce=IOC");

    auto formatResult = std::format("symbol={}&side={}&type={}&quantity={}&price={}&newClientOrderId={}&timestamp={}&newOrderRespType=RESULT&timeInForce={}",
            "BTCUSDT", "BUY", "LIMIT", 1.568, 44200.5, "cuniquestring", 1703789943256630, "IOC");
    EXPECT_EQ(formatResult, "symbol=BTCUSDT&side=BUY&type=LIMIT&quantity=1.568&price=44200.5&newClientOrderId=cuniquestring&timestamp=1703789943256630&newOrderRespType=RESULT&timeInForce=IOC");
}


TEST(BenchHFT, OKX) {
    using Aggr = Aggregator<StaticStr<R"({"ccy":)"_str>, QuotedStr<RuntimeStr<5>>,
        StaticStr<",\"clOrdId\":"_str>, QuotedStr<RuntimeStr<17>>,
        StaticStr<",\"sz\":"_str>, QuotedStr<RuntimeFloat<double>>,
        StaticStr<",\"side\":"_str>, QuotedStr<RuntimeStr<4>>,
        StaticStr<",\"px\":"_str>, QuotedStr<RuntimeFloat<double>>,
        StaticStr<",\"instId\":"_str>, QuotedStr<RuntimeStr<12>>,
        StaticStr<",\"ordType\":"_str>, QuotedStr<RuntimeStr<9>>,
        StaticStr<",\"tdMode\":"_str>, QuotedStr<RuntimeStr<5>>,
        StaticStr<"}"_str>>;
    auto maker = Maker<Aggr, LocalStorage, DebugConfig<>>();

    auto sv = maker.generate("USDT", "orderiduniquestr", 1.567, "buy", 44865.2, "BTC-USDT", "limit", "cross");
    EXPECT_EQ(sv, R"({"ccy":"USDT" ,"clOrdId":"orderiduniquestr" ,"sz":"1.567"             ,"side":"buy" ,"px":"44865.2"           ,"instId":"BTC-USDT"    ,"ordType":"limit"    ,"tdMode":"cross"})");

    auto formatStr = std::format(R"({{"ccy":"{}","clOrdId":"{}","sz":"{}","side":"{}","px":"{}","instId":"{}","ordType":"{}","tdMode":"{}"}})",
            "USDT", "orderiduniquestr", 1.567, "buy", 44865.2, "BTC-USDT", "limit", "cross");
    EXPECT_EQ(formatStr, R"({"ccy":"USDT","clOrdId":"orderiduniquestr","sz":"1.567","side":"buy","px":"44865.2","instId":"BTC-USDT","ordType":"limit","tdMode":"cross"})");
}
