#include "../helperTest.hpp"


TEST(Common, Integer) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, QuotedStr<RuntimeIntegral<int>, '*'>, StaticStr<"_end"_str>>, LocalStorage, DefaultConfig>();
    EXPECT_EQ(maker.generate(5), "start_*5*          _end");
    EXPECT_EQ(maker.generate(-2000000000), "start_*-2000000000*_end");
    // std::numeric_limits<int>::max() == 2147483647
    EXPECT_EQ(maker.generate(std::numeric_limits<int>::max()), "start_*2147483647* _end");
    // std::numeric_limits<int>::min() == -2147483648
    EXPECT_EQ(maker.generate(std::numeric_limits<int>::min()), "start_*-2147483648*_end");
}

