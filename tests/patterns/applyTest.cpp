#include "../helperTest.hpp"


int foo(int i) {
    return i * 2;
}

TEST(Apply, CStyleFunction) {

    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, Apply<RuntimeIntegral<int>, foo>, DynamicLocated<StaticStr<"_end"_str>>>>();

    EXPECT_EQ(maker.generate(2), "start_4_end");
}


template <typename T>
T bar(T i) {
    return i * 3;
}

TEST(Apply, TemplateFunction) {

    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, Apply<RuntimeIntegral<long>, bar<long>>, DynamicLocated<StaticStr<"_end"_str>>>>();

    EXPECT_EQ(maker.generate(3), "start_9_end");
}


struct Baz {
    auto operator()(std::string const& s) const {
        return s.substr(1);
    }
};

TEST(Apply, FunctionalObjectFunction) {

    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, Apply<RuntimeStr<10>, Baz{}>, DynamicLocated<StaticStr<"_end"_str>>>>();

    EXPECT_EQ(maker.generate("123"), "start_23_end");
}

