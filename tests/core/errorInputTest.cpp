#include "../helperTest.hpp"

template <typename Aggr, typename ...Args>
void runAllFailed(Args ...args) {
    std::tuple<DefaultConfig, DynamicConfig, PreInitConfig, PreInitOnlyConfig> tp;
    details::foreachTupleId([&]<IsConfig Cfg>(Cfg, auto) {
        auto maker = Maker<Aggr, LocalStorage, Cfg>{};
        EXPECT_EQ(maker.generate(args...), "");
    }, tp);
}

template <typename Aggr>
using MakeMaker = Maker<Aggr, LocalStorage, DynamicConfig>;

TEST(ErrorInput, Common) {
    using Aggr = MakeAggregator<"Test{}"_str, RuntimeStr<5>>;
    auto maker = MakeMaker<Aggr>{};
    EXPECT_EQ(maker.generate("666666"), "");
    runAllFailed<Aggr>("666666");
}

TEST(ErrorInput, Common2) {
    using Aggr = MakeAggregator<"Test{}{}"_str, RuntimeStr<5>, RuntimeStr<1>>;
    runAllFailed<Aggr>("666666", "1");
    runAllFailed<Aggr>("66666", "22");
}

TEST(ErrorInput, ApplyIntToStr) {
    using Aggr = MakeAggregator<"Test{}{}"_str, Apply<RuntimeStr<5>, [](int i){
        return std::to_string(i);
    }>, RuntimeStr<1>>;

    auto maker = MakeMaker<Aggr>{};
    EXPECT_EQ(maker.generate(6, "1"), "Test61");
    runAllFailed<Aggr>(6, "11");
    runAllFailed<Aggr>(66666, "22");
}

TEST(ErrorInput, ApplyIntToInt) {
    using Aggr = MakeAggregator<"Test{}{}"_str, Apply<RuntimeIntegral<unsigned, 4>, [](int i){
        return i * i;
    }>, RuntimeStr<1>>;

    auto maker = MakeMaker<Aggr>{};
    EXPECT_EQ(maker.generate(6, "a"), "Test36a");
    runAllFailed<Aggr>(100, "2");
    runAllFailed<Aggr>(100, "22");
}