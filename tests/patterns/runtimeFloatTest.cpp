#include "../helperTest.hpp"


TEST(Common, Double) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, QuotedStr<RuntimeFloat<double>, '*'>, StaticStr<"_end"_str>>, LocalStorage, DefaultConfig>();
    EXPECT_EQ(maker.generate(5),             "start_*5*                 _end");
    EXPECT_EQ(maker.generate(1.5),           "start_*1.5*               _end");
    EXPECT_EQ(maker.generate(0),             "start_*0*                 _end");

    // std::numeric_limits<double>::max() > 1e300 cannot be printed
    EXPECT_EQ(maker.generate(std::numeric_limits<double>::max()), "");
    EXPECT_EQ(maker.generate(100000000000000000.5),
            "start_*1e+17*             _end");

    EXPECT_EQ(maker.generate(1.0/3),         "start_*0.3333333333333333*_end");

    EXPECT_EQ(maker.generate(1.0/3 - 10000), "start_*-9999.666666666666*_end");
}
