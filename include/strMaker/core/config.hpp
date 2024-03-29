#pragma once

#include "../utils/constexprString.hpp"

#include <cassert>
#include <source_location>
#include <iostream>
#include <cstring>


namespace mkr {

enum class AggregatorMode {
    DEFAULT = 0,
    PRE_INIT = 1,
    PRE_INIT_ONLY = 2,
    DYNAMIC = 3,
};

template <char neutral = ' ', char error = '~', AggregatorMode modeValue = AggregatorMode::DEFAULT>
struct Config {
    constexpr Config() = default;

    static constexpr char neutralSymbol() noexcept {
        return neutral;
    }

    static constexpr char errorSymbol() noexcept {
        return error;
    }

    static constexpr AggregatorMode mode() noexcept {
        return modeValue;
    }

    void errorEvent() const noexcept {

    }

    template <size_t N>
    void fill(char* __restrict__ ptr, ConstexprString<N> const& str) const noexcept {
        memcpy(ptr, str.c_str(), N);
    }

    void memset(char* ptr, char c, size_t size) const {
        ::memset(ptr, c, size);
    }

    void memcpy(char* __restrict__ dst, char const*const __restrict__ src, size_t size) const {
        ::memcpy(dst, src, size);
    }
};


template<typename T>
concept IsConfig = requires(T a, char* ptr, const char* const src, size_t size) {
    { a.neutralSymbol() } -> std::same_as<char>;
    { a.errorSymbol() } -> std::same_as<char>;
    { a.mode() } -> std::same_as<AggregatorMode>;
    { a.errorEvent() };
    { a.fill(ptr, ConstexprString<>::fromChar('a')) };
    { a.memset(ptr, 'a', size) };
    { a.memcpy(ptr, src, size) };
};

template <char neutral = ' ', char error = '~', AggregatorMode mode = AggregatorMode::DEFAULT>
struct DebugConfig : public Config<neutral, error, mode> {
    void errorEvent(std::source_location const& location = std::source_location::current()) const {
        std::cout << "Error: " << location.file_name() << ":" << location.line()
            << " [" << location.function_name() << "]" << std::endl;
    }
};


using DefaultConfig = Config<>;
using DynamicConfig = Config<' ', '~', AggregatorMode::DYNAMIC>;
using PreInitConfig = Config<' ', '~', AggregatorMode::PRE_INIT>;
using PreInitOnlyConfig = Config<' ', '~', AggregatorMode::PRE_INIT_ONLY>;

}