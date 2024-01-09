#pragma once

#include "../core/memory.hpp"
#include "../utils/template.hpp"
#include "../utils/numberHelper.hpp"

#include <charconv>

namespace mkr {

template <std::integral Number, IndexT maxSizeSuggestion = 0>
class RuntimeIntegral {
public:
    static constexpr bool permanent = false;
    static constexpr bool needVariable = true;
    static constexpr IndexT maxSize = maxSizeSuggestion == 0 ? details::maxNumberSize<Number>() : maxSizeSuggestion;

    template <IndexT Size, IsConfig Config>
    void initMemory(Memory<Size>& mem, IndexT pos, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, pos, cfg);
    }

    template <IndexT memSize, IsConfig Config>
    IndexT generate(Memory<memSize>& mem, IndexT pos, Config const& cfg, Number n) const noexcept {

        auto result = std::to_chars(mem.data() + pos, mem.data() + pos + maxSize, n);
        if (result.ec != std::errc{}) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }
        return result.ptr - mem.data();
    }
};

}