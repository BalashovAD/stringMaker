#include "../helperTest.hpp"

using Cfg = Config<'!', '#', AggregatorMode::DYNAMIC>;

TEST(DynamicConfig, Common) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage, Cfg>();
    EXPECT_EQ(maker.generate("@@@"), "start_@@@_end");
    EXPECT_EQ(maker.generate("@@@@@@"), "");
}

TEST(DynamicConfig, RuntimeRuntime) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage, Cfg>();
    EXPECT_EQ(maker.generate("@@@", "$$$"), "start_@@@$$$_end");
    EXPECT_EQ(maker.generate("@@@@@", "$$$$$"), "start_@@@@@$$$$$_end");
}

TEST(DynamicConfig, RuntimeRuntimeAtTheEnd) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, RuntimeStr<5>>, LocalStorage, Cfg>();
    EXPECT_EQ(maker.generate("@@@", "$$$"), "start_@@@$$$");
    EXPECT_EQ(maker.generate("@@@@@", "$$$$$"), "start_@@@@@$$$$$");
}

TEST(DynamicConfig, FirstRuntimeRuntimeAtTheEnd) {
    auto maker = Maker<Aggregator<RuntimeStr<5>, RuntimeStr<5>>, LocalStorage, Cfg>();
//    EXPECT_EQ(maker.generate("@@@", "$$$"), "@@@$$$");
//    EXPECT_EQ(maker.generate("@@@@@", "$$$$$"), "@@@@@$$$$$");
    EXPECT_EQ(maker.generate("@@@@@@", "$$$$$"), "");
    EXPECT_EQ(maker.generate("@@@@@", "$$$$$$"), "");
}

