#pragma once

#include <typeinfo>
#include <type_traits>
#include <memory>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include <gtest/gtest.h>

#include <strMaker/all.hpp>


inline std::string demangle(const char* name) {
#ifdef __GNUC__
    int status = -1;
    std::unique_ptr<char, void(*)(void*)> res{
            abi::__cxa_demangle(name, nullptr, nullptr, &status),
            std::free
    };
    return (status == 0) ? res.get() : name;
#else
    return name;
#endif
}

template <typename T>
void printType() {
    std::cout << demangle(typeid(T).name()) << std::endl;
}


using namespace testing;
using namespace mkr;

DISABLE_WARNING_PUSH
DISABLE_WARNING(-Wunused-parameter)

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
