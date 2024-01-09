#include "../helperTest.hpp"


TEST(CustomType, CustomOwnClass) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, QuotedStr<CustomType<MyStruct>, '*'>, StaticStr<"_end"_str>>, LocalStorage, DefaultConfig>();

    MyStruct t{5, "test"};
    EXPECT_EQ(maker.generate(t), "start_*5test*               _end");
}


template<>
struct details::CustomMaxSize<std::optional<int>> {
    static constexpr IndexT maxSize = std::max(RuntimeIntegral<int>::maxSize, sizeof("null"));
};

namespace std {

inline char* mkr_generate(char* begin, char* end, DefaultConfig const& cfg, std::optional<int> const& t) {
    auto nullSize = sizeof("null") - 1;
    assert(std::cmp_greater(end - begin, nullSize));
    if (t) {
        auto result = std::to_chars(begin, end, *t);
        return result.ptr;
    } else {
        cfg.memcpy(begin, "null", nullSize);
        return begin + nullSize;
    }
}

}

TEST(CustomType, Inject) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, QuotedStr<CustomType<std::optional<int>>, '*'>, StaticStr<"_end"_str>>, LocalStorage, DefaultConfig>();

    std::optional<int> t{};
    EXPECT_EQ(maker.generate(t), "start_*null*       _end");
    t = 5;
    EXPECT_EQ(maker.generate(t), "start_*5*          _end");
}

