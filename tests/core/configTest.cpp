#include "../helperTest.hpp"


TEST(Config, CustomConfig) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage, DebugConfig<'!', '#'>>();
    EXPECT_EQ(maker.generate("@@@"), "start_@@@!!_end");
    EXPECT_EQ(maker.generate("@@@@@@"), "");
}

