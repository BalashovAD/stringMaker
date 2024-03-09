#include "../helperTest.hpp"

using Cfg = PreInitOnlyConfig;

TEST(PreInitOnly, Local) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage, Cfg>();
    auto sv1 = maker.generate("@@@");
    EXPECT_EQ(sv1, "start_@@@  _end");
    std::string_view sv2;
    EXPECT_DEBUG_DEATH(sv2 = maker.generate("!!"), ""); // no pre-init

    EXPECT_NE(sv2, "start_!!   _end");
}

TEST(PreInitOnly, LocalPreInit) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage, Cfg>();
    auto sv1 = maker.generate("@@@");
    EXPECT_EQ(sv1, "start_@@@  _end");
    maker.init();
    auto sv2 = maker.generate("!!"); // pre-init
    EXPECT_EQ(sv2, "start_!!   _end");
}

TEST(PreInitOnly, Ring) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, RingBufferStorageWrapper<2, false>::type, Cfg>();
    auto sv1 = maker.generate("@@@");
    auto sv2 = maker.generate("!!"); // no pre-init RING ok

    EXPECT_EQ(sv1, "start_@@@  _end");
    EXPECT_EQ(sv2, "start_!!   _end");
}

TEST(PreInitOnly, RingError) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, RingBufferStorageWrapper<2, false>::type, Cfg>();
    auto sv1 = maker.generate("@@@");
    auto sv2 = maker.generate("!!"); // no pre-init RING ok
    EXPECT_EQ(sv1, "start_@@@  _end");
    EXPECT_EQ(sv2, "start_!!   _end");

    std::string_view sv3;
    EXPECT_DEBUG_DEATH(sv3 = maker.generate("!!"), ""); // need pre-init
    EXPECT_NE(sv3, "start_!!   _end");
}

TEST(PreInitOnly, RingPreInit) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, RingBufferStorageWrapper<2, false>::type, Cfg>();
    auto sv1 = maker.generate("@@@");
    EXPECT_EQ(sv1, "start_@@@  _end");

    maker.init();
    auto sv2 = maker.generate("!!"); // early pre-init

    EXPECT_EQ(sv2, "start_!!   _end");
}


TEST(PreInitOnly, RingPreInitAfterSecond) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, RingBufferStorageWrapper<2, false>::type, Cfg>();

    {
        auto sv1 = maker.generate("@@@");
        auto sv2 = maker.generate("!!");

        EXPECT_EQ(sv1, "start_@@@  _end");
        EXPECT_EQ(sv2, "start_!!   _end");
    }
    {
        maker.init();
        auto sv1 = maker.generate("@@@");
        auto sv2 = maker.generate("!!");

        EXPECT_EQ(sv1, "start_@@@  _end");
        EXPECT_EQ(sv2, "start_!!   _end");
    }
}
