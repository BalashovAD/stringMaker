#include "../helperTest.hpp"

TEST(ErrorHandler, RuntimeStrStaticStr) {

    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, ErrorHandler<RuntimeStr<10>, StaticStr<"longstr"_str>>, DynamicLocated<StaticStr<"_end"_str>>>>();

    EXPECT_EQ(maker.generate("test"), "start_test_end");
    EXPECT_EQ(maker.generate("testtestte"), "start_testtestte_end");
    EXPECT_EQ(maker.generate("verylongstring"), "start_longstr_end");
}


TEST(ErrorHandler, RuntimeStrRuntimeStr) {

    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, ErrorHandler<RuntimeStr<10>, RuntimeStr<15>>, DynamicLocated<StaticStr<"_end"_str>>>>();

    EXPECT_EQ(maker.generate("test"), "start_test_end");
    EXPECT_EQ(maker.generate("testtestte"), "start_testtestte_end");
    EXPECT_EQ(maker.generate("verylongstring"), "start_verylongstring_end");
    EXPECT_EQ(maker.generate("verylongstringverylongstring"), "");
}


std::string cut(int i) {
    return std::to_string(i / 100);
}

TEST(ErrorHandler, NumberCuttedNumber) {

    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, ErrorHandler<RuntimeIntegral<int, 3>, Apply<RuntimeStr<3>, cut>>, DynamicLocated<StaticStr<"_end"_str>>>>();

    EXPECT_EQ(maker.generate(10), "start_10_end");
    EXPECT_EQ(maker.generate(100), "start_100_end");
    EXPECT_EQ(maker.generate(1000), "start_10_end");
    EXPECT_EQ(maker.generate(10000), "start_100_end");
    EXPECT_EQ(maker.generate(1000000), "");
}