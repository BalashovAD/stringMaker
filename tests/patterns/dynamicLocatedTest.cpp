#include "../helperTest.hpp"


TEST(DynamicLocated, StaticStr) {

    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, QuotedStr<RuntimeStr<10>, '*'>, DynamicLocated<StaticStr<"_end"_str>>>>();

    EXPECT_EQ(maker.generate("test"), "start_*test*_end");
    EXPECT_EQ(maker.generate("testtestte"), "start_*testtestte*_end");
}