#pragma once

#include "pattern.hpp"

#include "../utils/template.hpp"

namespace mkr {

template <IsPattern T>
class DynamicLocated : private T {
public:
    static constexpr bool permanent = false;
    static constexpr bool needVariable = details::checkNeedVariable<T>();
    static constexpr bool dynamicLocated = true;
    static constexpr IndexT maxSize = details::getSize<T>();

    template <IndexT Size, IsConfig Config>
    void initMemory(Memory<Size>& mem, IndexT pos, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, pos, cfg);
    }

    template <IndexT memSize, IsConfig Config, typename Arg>
        requires(needVariable)
    IndexT generate(Memory<memSize>& mem, IndexT pos, Config const& cfg, Arg&& n) const {

        if constexpr (details::isPermanent<T>()) {
            T::initMemory(mem, pos, cfg);
            return pos + maxSize;
        } else {
            return T::generate(mem, pos, cfg, n);
        }
    }

    template <IndexT memSize, IsConfig Config>
        requires(!needVariable)
    IndexT generate(Memory<memSize>& mem, IndexT pos, Config const& cfg) const noexcept {

        if constexpr (details::isPermanent<T>()) {
            T::initMemory(mem, pos, cfg);
            return pos + maxSize;
        } else {
            return T::generate(mem, pos, cfg);
        }
    }
};

}