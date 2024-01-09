#include "../helperTest.hpp"

TEST(LocalStorage, DoubleCall) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage>();
    auto sv1 = maker.generate("@@@");
    EXPECT_EQ(sv1, "start_@@@  _end");
    auto sv2 = maker.generate("@@@@@");
    EXPECT_EQ(sv2, "start_@@@@@_end");
    EXPECT_EQ(sv1, "start_@@@@@_end");
}

TEST(LocalStorage, ErrorInCallUnpredictable) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage>();
    auto sv1 = maker.generate("@@@");
    EXPECT_EQ(sv1, "start_@@@  _end");
    auto sv2 = maker.generate("@@@@@@@"); // error
    EXPECT_EQ(sv2, "");
    EXPECT_EQ(sv1, "start_@@@  _end"); // unknown
}

TEST(LocalStorage, ErrorInCallUnpredictableAnotherWay) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage>();
    auto sv1 = maker.generate("@@@", "!!!");
    EXPECT_EQ(sv1, "start_@@@  !!!  _end");
    auto sv2 = maker.generate("@@@@@", "!!!!!!"); // error
    EXPECT_EQ(sv2, "");
    EXPECT_EQ(sv1, "start_@@@@@!!!  _end"); // unknown
}