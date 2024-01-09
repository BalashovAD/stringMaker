#include "../helperTest.hpp"

struct MyStructThrow {
    int a;
    std::string s;

    static constexpr IndexT mkr_maxSize = 20;

    friend char* mkr_generate(char* begin, char* end, DefaultConfig const& cfg, MyStructThrow const& t) noexcept(false) {
        throw std::runtime_error("test");
    }
};


TEST(ExceptionSafety, CustomClass) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, DynamicLocated<QuotedStr<CustomType<MyStructThrow>, '*'>>, StaticStr<"_end"_str>>, LocalStorage, DefaultConfig>();

    MyStructThrow t{5, "test"};
    EXPECT_ANY_THROW(maker.generate(t));
}