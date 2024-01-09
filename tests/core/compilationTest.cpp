#include "../helperTest.hpp"

#include <type_traits>

TEST(CompilationTest, WrongArgsNumber) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, RuntimeStr<5>, StaticStr<"_end"_str>>, LocalStorage, DebugConfig<'!', '#'>>();

//    maker.generate();
//    maker.generate(4);
    maker.generate("test");
//    maker.generate("test", "test");
//    maker.generate("test", 5);
}



struct MyStruct2 {
    int a;
    std::string s;

    static constexpr IndexT mkr_maxSize = 20;

    friend char* mkr_generate(char* begin, char* end, DefaultConfig const& cfg, MyStruct2 const& t) noexcept {
        auto result = std::to_chars(begin, end, t.a);
        if (result.ec != std::errc{}) [[unlikely]] {
            return nullptr;
        }
        if (result.ptr + t.s.size() < end) {
            cfg.memcpy(result.ptr, t.s.data(), t.s.size());
            return result.ptr + t.s.size();
        } else {
            return nullptr;
        }
    }
};


struct MyStruct3 {
    int a;
    std::string s;

    operator MyStruct() const {
        return MyStruct{a, s};
    }
};


struct MyStruct4 {
    int a;
    std::string s;

    explicit operator MyStruct() const {
        return MyStruct{a, s};
    }
};


struct MyStruct5 {
    int a;
    std::string s;

    explicit operator MyStruct() const {
        return MyStruct{a, s};
    }

    static constexpr IndexT mkr_maxSize = 20;

    friend char* mkr_generate(char* begin, char* end, DefaultConfig const& cfg, MyStruct4 const& t) {
        throw std::runtime_error("wrong method");
    }
};



TEST(CompilationTest, WrongType) {
    auto maker = Maker<Aggregator<StaticStr<"start_"_str>, QuotedStr<CustomType<MyStruct>, '*'>, StaticStr<"_end"_str>>, LocalStorage, DefaultConfig>();

    static_assert(details::explicitly_convertible_to<MyStruct3, MyStruct>);
    static_assert(details::explicitly_convertible_to<MyStruct4, MyStruct>);
    maker.generate(MyStruct{4, "test"});
//    maker.generate(MyStruct2{5, "test"});
    maker.generate(MyStruct3{5, "test"}); // implicit cast
    maker.generate(MyStruct4{5, "test"}); // explicit cast
    EXPECT_NO_THROW(maker.generate(MyStruct5{5, "test"})); // should call MyStruct::mkr_generate
}

static_assert(IsStorage<RingBufferStorage<3, false, 10>>);


