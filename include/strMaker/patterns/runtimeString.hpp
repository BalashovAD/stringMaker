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

    template <IndexT Size, IsConfig Config>
    void initMemory(Memory<Size>& mem, IndexT pos, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, pos, cfg);
    }

    template <IndexT memSize, IsConfig Config>
    IndexT generate(Memory<memSize>& mem, IndexT pos, Config const& cfg, std::string_view s) const noexcept {
        if (s.size() > maxSize) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }
        cfg.memcpy(mem.data() + pos, s.data(), s.size());
        return pos + s.size();
    }
};


}