#pragma once

#include "../core/memory.hpp"
#include "../utils/constexprString.hpp"

namespace mkr {

template <ConstexprString str>
class StaticStr {
public:
    static constexpr bool permanent = true;
    static constexpr IndexT maxSize = str.size();

    template <size_t Size, typename Config>
    void initMemory(Memory<Size>& mem, IndexT pos, Config const& cfg) const noexcept {
        cfg.fill(mem.data() + pos, str);
    }
};


}