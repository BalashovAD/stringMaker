#include "../helperTest.hpp"

using Cfg = PreInitConfig;

TEST(PreInit, Local) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage, Cfg>();
    auto sv1 = maker.generate("@@@");
    EXPECT_EQ(sv1, "start_@@@  _end");

    auto sv2 = maker.generate("!!"); // no pre-init
    EXPECT_EQ(sv2, "start_!!   _end");
}

TEST(PreInit, LocalPreInit) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage, Cfg>();
    auto sv1 = maker.generate("@@@");
    EXPECT_EQ(sv1, "start_@@@  _end");
    maker.init();
    auto sv2 = maker.generate("!!"); // pre-init
    EXPECT_EQ(sv2, "start_!!   _end");
}
