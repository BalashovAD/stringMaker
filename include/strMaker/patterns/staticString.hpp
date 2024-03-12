#pragma once

#include "../core/memory.hpp"
#include "../utils/constexprString.hpp"

namespace mkr {

template <ConstexprString strP>
class StaticStr {
public:
    static constexpr auto str = strP;
    static constexpr bool permanent = true;
    static constexpr IndexT maxSize = str.size();

    template <typename Config>
    void initMemory(CharIt mem, [[maybe_unused]] CharEnd end, Config const& cfg) const noexcept {
        cfg.fill(mem, str);
    }
};


}