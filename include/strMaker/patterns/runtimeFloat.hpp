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

    template <IndexT Size, IsConfig Config>
    void initMemory(Memory<Size>& mem, IndexT pos, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, pos, cfg);
    }

    template <IndexT memSize, IsConfig Config>
    IndexT generate(Memory<memSize>& mem, IndexT pos, Config const& cfg, Number n) const noexcept {

        if (std::isnan(n)) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }

        auto result = std::to_chars(mem.data() + pos, mem.data() + pos + maxSize, n);
        if (result.ec != std::errc{}) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }
        return result.ptr - mem.data();
    }
};


}