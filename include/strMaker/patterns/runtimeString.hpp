#pragma once

#include "../core/memory.hpp"
#include "../utils/constexprString.hpp"
#include "../core/config.hpp"

namespace mkr {

template <IndexT maxSizeParam>
class RuntimeStr {
public:
    static constexpr bool permanent = false;
    static constexpr bool needVariable = true;
    static constexpr IndexT maxSize = maxSizeParam;

    template <IsConfig Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, end, cfg);
    }

    template <IsConfig Config>
    IndexT generate(CharIt mem, [[maybe_unused]] CharEnd end, Config const& cfg, std::string_view s) const noexcept {
        if (s.size() > maxSize) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }
        cfg.memcpy(mem, s.data(), s.size());
        return s.size();
    }
};


}