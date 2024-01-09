#include "../helperTest.hpp"


TEST(Quoted, QuotedRuntimeStr) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, QuotedStr<RuntimeStr<5>, '*'>, StaticStr<"_end"_str>>, LocalStorage, DefaultConfig>();
    EXPECT_EQ(maker.generate("@@@"), "start_*@@@*  _end");
    EXPECT_EQ(maker.generate("@@@@@"), "start_*@@@@@*_end");
    EXPECT_EQ(maker.generate("@@@@@@"), "");
}


TEST(Quoted, QuotedQuotedRuntimeStr) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, QuotedStr<QuotedStr<RuntimeStr<5>, '*'>, '&'>, StaticStr<"_end"_str>>, LocalStorage, DefaultConfig>();
    EXPECT_EQ(maker.generate("@@@"), "start_&*@@@*&  _end");
    EXPECT_EQ(maker.generate("@@@@@"), "start_&*@@@@@*&_end");
    EXPECT_EQ(maker.generate("@@@@@@"), "");
}
