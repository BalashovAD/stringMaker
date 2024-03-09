#include "../helperTest.hpp"

#include <type_traits>

TEST(MakeAggregator, Common) {
    using Aggr = MakeAggregator<"abc{}def"_str, RuntimeIntegral<int>>;
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(4), "abc4def");
}

TEST(MakeAggregator, Common2) {
    using Aggr = MakeAggregator<"abc{}def{}gh"_str, RuntimeIntegral<int>, RuntimeStr<3>>;
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(4, "!!"), "abc4def!!gh");
}

TEST(MakeAggregator, NoSpace) {
    using Aggr = MakeAggregator<"abc{}{}gh"_str, RuntimeIntegral<int>, RuntimeStr<3>>;
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(4, "!!"), "abc4!!gh");
}

TEST(MakeAggregator, NoSpaceAtTheEnd) {
    using Aggr = MakeAggregator<"abc{}def{}"_str, RuntimeIntegral<int>, RuntimeStr<3>>;
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(4, "!!"), "abc4def!!");
}

TEST(MakeAggregator, NoArgs) {
    using Aggr = MakeAggregator<"abcdef"_str>;
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(), "abcdef");
}

TEST(MakeAggregator, BrokenWrongArgumentNumber) {
//    using Aggr = MakeAggregator<"abc{def}"_str, RuntimeStr<2>>;
//
//    using Aggr = MakeAggregator<"abc{}def{"_str, RuntimeStr<2>, RuntimeStr<2>>;
//
//    using Aggr = MakeAggregator<"abc{}def{}"_str, RuntimeStr<2>>;
//
//    using Aggr = MakeAggregator<"abc{}def"_str>;
}

