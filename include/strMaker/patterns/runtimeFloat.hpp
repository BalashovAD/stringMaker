#pragma once

#include "../core/memory.hpp"
#include "../utils/template.hpp"
#include "../utils/numberHelper.hpp"

#include <charconv>
#include <cmath>


namespace mkr {

template <std::floating_point Number = double, size_t maxPossibleSize = 0>
class RuntimeFloat {
public:
    static constexpr bool permanent = false;
    static constexpr bool needVariable = true;
    static constexpr IndexT maxSize = maxPossibleSize == 0 ? details::maxNumberSize<Number>() : maxPossibleSize;

    template <IsConfig Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, end, cfg);
    }

    template <IsConfig Config>
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg, Number n) const noexcept {

        if (std::isnan(n)) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }

        auto result = std::to_chars(mem, mem + maxSize, n);
        if (result.ec != std::errc{}) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }
        return result.ptr - mem;
    }
};


}