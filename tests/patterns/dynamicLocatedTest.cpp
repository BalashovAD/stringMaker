#include "../helperTest.hpp"


TEST(DynamicLocated, StaticStr) {

    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, QuotedStr<RuntimeStr<10>, '*'>, DynamicLocated<StaticStr<"_end"_str>>>>();

    EXPECT_EQ(maker.generate("test"), "start_*test*_end");
    EXPECT_EQ(maker.generate("testtestte"), "start_*testtestte*_end");
}

TEST(DynamicLocated, StaticStrDynDyn) {
    // useless first dynamicLocated
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, DynamicLocated<QuotedStr<RuntimeStr<10>, '*'>>, DynamicLocated<StaticStr<"_end"_str>>>>();

    EXPECT_EQ(maker.generate("test"), "start_*test*_end");
    EXPECT_EQ(maker.generate("testtestte"), "start_*testtestte*_end");
}

TEST(DynamicLocated, DynDyn) {
    // useless first dynamicLocated
    auto maker = Maker<Aggregator<RuntimeStr<3>, DynamicLocated<QuotedStr<RuntimeStr<10>, '*'>>, DynamicLocated<StaticStr<"_end"_str>>>>();

    EXPECT_EQ(maker.generate("St", "test"), "St*test*_end");
    EXPECT_EQ(maker.generate("St_", "test"), "St_*test*_end");
    EXPECT_EQ(maker.generate("Start", "test"), "");
    EXPECT_EQ(maker.generate("St", "testtestte"), "St*testtestte*_end");
}