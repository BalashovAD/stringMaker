#include "../helperTest.hpp"

#include <type_traits>

TEST(MakeAggregator, Common) {
    using Aggr = MakeAggregator<"abc{}def"_str, RuntimeIntegral<int>>;
    static_assert(Aggr::patternSize == 3);
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(4), "abc4def");
}

TEST(MakeAggregator, Common2) {
    using Aggr = MakeAggregator<"abc{}def{}gh"_str, RuntimeIntegral<int>, RuntimeStr<3>>;
    static_assert(Aggr::patternSize == 5);
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(4, "!!"), "abc4def!!gh");
}

TEST(MakeAggregator, NoSpace) {
    using Aggr = MakeAggregator<"abc{}{}gh"_str, RuntimeIntegral<int>, RuntimeStr<3>>;
    static_assert(Aggr::patternSize == 4);
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(4, "!!"), "abc4!!gh");
}

TEST(MakeAggregator, NoSpaceAtTheEnd) {
    using Aggr = MakeAggregator<"abc{}def{}"_str, RuntimeIntegral<int>, RuntimeStr<3>>;
    static_assert(Aggr::patternSize == 4);
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(4, "!!"), "abc4def!!");
}

TEST(MakeAggregator, NoArgs) {
    using Aggr = MakeAggregator<"abcdef"_str>;
    static_assert(Aggr::patternSize == 1);
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(), "abcdef");
}


TEST(MakeAggregator, SingleStaticStr) {
    using Aggr = MakeAggregator<"{}"_str, StaticStr<"1"_str>>;
    static_assert(Aggr::patternSize == 1);
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(), "1");
}


TEST(MakeAggregator, CompressStaticStrings) {
    using Aggr = MakeAggregator<"Test{}Test"_str, StaticStr<"1"_str>>;
    static_assert(Aggr::patternSize == 1);
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(), "Test1Test");
}


TEST(MakeAggregator, CompressStaticStrings2) {
    using Aggr = MakeAggregator<"Test{}Test{}"_str, StaticStr<"1"_str>, StaticStr<"2"_str>>;
    static_assert(Aggr::patternSize == 1);
    auto maker = Maker<Aggr, LocalStorage, DynamicConfig>{};
    EXPECT_EQ(maker.generate(), "Test1Test2");
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

