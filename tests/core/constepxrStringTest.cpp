#include "../helperTest.hpp"

#include <type_traits>

TEST(ConstexprString, Common) {
    auto str = "test"_str;
    EXPECT_EQ(str.sv(), "test");
}

TEST(ConstexprString, RawString) {
    auto str = R"(te"st)"_str;
    EXPECT_EQ(str.sv(), "te\"st");
}

TEST(ConstexprString, Repeat) {
    auto str = ("test"_str).repeat<2>();
    EXPECT_EQ(str.sv(), "testtest");
}

TEST(ConstexprString, RepeatChar) {
    auto str = repeatStr<3>('c');
    EXPECT_EQ(str.sv(), "ccc");
}

TEST(ConstexprString, FromChar) {
    auto str = ConstexprString<>::fromChar('c');
    EXPECT_EQ(str.sv(), "c");
}

TEST(ConstexprString, Find) {
    auto constexpr str = "abcdef"_str;
    static_assert(str.find("a"_str) == 0);
    static_assert(str.find("f"_str) == 5);
    static_assert(str.find("ab"_str) == 0);
    static_assert(str.find("bc"_str) == 1);
    static_assert(str.find("ac"_str) == std::string::npos);
    static_assert(str.find("ff"_str) == std::string::npos);
    static_assert((""_str).find("a"_str) == std::string::npos);
    static_assert((""_str).find("aa"_str) == std::string::npos);
//    str.find(""_str); // Compilation error
}