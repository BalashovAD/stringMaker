#pragma once

#include <gtest/gtest.h>

#include "../include/strMaker/full.hpp"

using namespace testing;
using namespace mkr;


struct MyStruct {
    int a;
    std::string s;

    static constexpr IndexT mkr_maxSize = 20;

    friend char* mkr_generate(char* begin, char* end, DefaultConfig const& cfg, MyStruct const& t) noexcept {
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

