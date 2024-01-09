#include "../helperTest.hpp"


TEST(RuntimeStr, PatternCombo) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage, DefaultConfig>();
    EXPECT_EQ(maker.generate("@@@"), "start_@@@  _end");
    EXPECT_EQ(maker.generate("@@@@@@"), "");
}


TEST(RuntimeStr, PatternRuntimeRuntime) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage, DefaultConfig>();
    EXPECT_EQ(maker.generate("@@@", "###"), "start_@@@  ###  _end");
    EXPECT_EQ(maker.generate("@@@", "######"), "");
}

