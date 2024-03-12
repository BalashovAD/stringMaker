#pragma once

#include "../core/memory.hpp"
#include "../utils/template.hpp"
#include "../utils/numberHelper.hpp"

#include <charconv>
#include <concepts>

namespace mkr {

template <std::integral Number, IndexT maxSizeSuggestion = 0>
class RuntimeIntegral {
public:
    static constexpr bool permanent = false;
    static constexpr bool needVariable = true;
    static constexpr IndexT maxSize = maxSizeSuggestion == 0 ? details::maxNumberSize<Number>() : maxSizeSuggestion;

    template <IsConfig Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, end, cfg);
    }

    template <IsConfig Config>
    IndexT generate(CharIt mem, [[maybe_unused]] CharEnd end, Config const& cfg, Number n) const noexcept {

        auto result = std::to_chars(mem, mem + maxSize, n);
        if (result.ec != std::errc{}) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }
        return result.ptr - mem;
    }
};

}