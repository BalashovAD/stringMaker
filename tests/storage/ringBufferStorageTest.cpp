#include "../helperTest.hpp"

TEST(RingBufferStorage, DoubleCall) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, RingBufferStorageWrapper<3, false>::type>();
    auto sv1 = maker.generate("@@@");
    EXPECT_EQ(sv1, "start_@@@  _end");
    auto sv2 = maker.generate("@@@@@");
    EXPECT_EQ(sv2, "start_@@@@@_end");
    EXPECT_EQ(sv1, "start_@@@  _end");
    auto sv3 = maker.generate("");
    EXPECT_EQ(sv3, "start_     _end");
    EXPECT_EQ(sv2, "start_@@@@@_end");
    EXPECT_EQ(sv1, "start_@@@  _end");
    auto sv4 = maker.generate(""); // overflow
    EXPECT_EQ(sv4, "start_     _end");
    EXPECT_EQ(sv3, "start_     _end");
    EXPECT_EQ(sv2, "start_@@@@@_end");
    EXPECT_EQ(sv1, "start_     _end");
}


TEST(RingBufferStorage, StrictCheck) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, RingBufferStorageWrapper<2, true>::type>();
    auto sv1 = maker.generate("@@@");
    auto sv2 = maker.generate("@@@@@");
    EXPECT_EQ(sv2, "start_@@@@@_end");
    EXPECT_EQ(sv1, "start_@@@  _end");
    EXPECT_ANY_THROW(maker.generate(""));
    EXPECT_EQ(sv2, "start_@@@@@_end");
    EXPECT_EQ(sv1, "start_@@@  _end");
}


TEST(RingBufferStorage, StrictCheckRelease) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, RingBufferStorageWrapper<2, true>::type>();
    auto sv1 = maker.generate("@@@");
    auto sv2 = maker.generate("@@@@@");
    EXPECT_EQ(sv2, "start_@@@@@_end");
    EXPECT_EQ(sv1, "start_@@@  _end");
    maker.releaseMemory();
    maker.generate("");
    EXPECT_EQ(sv2, "start_@@@@@_end");
    EXPECT_EQ(sv1, "start_     _end"); // undefined
}
