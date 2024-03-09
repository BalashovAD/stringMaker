#pragma once

#include "../core/memory.hpp"
#include "../utils/constexprString.hpp"

namespace mkr {

template <ConstexprString str>
class StaticStr {
public:
    static constexpr bool permanent = true;
    static constexpr IndexT maxSize = str.size();

    template <typename Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        cfg.fill(mem, str);
    }
};


}