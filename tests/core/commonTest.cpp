#include "../helperTest.hpp"


TEST(Common, TwoPatterns) {
    using StaticSS = StaticStr<"test"_str>;

    static_assert(StaticSS::maxSize == 4);
    static_assert(details::getSize<StaticSS>() == 4);
    static_assert(RuntimeStr<10>::maxSize == 10);
    static_assert(details::getSize<RuntimeStr<10>>() == 10);
    static_assert(details::isPermanent<StaticSS>());

    constexpr auto info = details::helper::makeInfo<StaticSS>(0, 0);

    static_assert(info.varInd == -1);
    static_assert(info.pos == 0);
    static_assert(info.needCall == false);
    static_assert(info.size == 4);

    auto maker = Maker<Aggregator<StaticStr<"test"_str>, RuntimeStr<10>>, LocalStorage>();
    EXPECT_EQ(maker.generate("_test_"), "test_test_");
    EXPECT_EQ(maker.generate("aaaaaaaaaa"), "testaaaaaaaaaa");
    EXPECT_EQ(maker.generate("aaaaaaaaaaaaaaaaaaaa"), "");
}

