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

    template <IsConfig Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, end, cfg);
    }

    template <IsConfig Config, typename Arg>
        requires(needVariable)
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg, Arg&& n) const {

        if constexpr (details::isPermanent<T>()) {
            T::initMemory(mem, end, cfg);
            return maxSize;
        } else {
            return T::generate(mem, end, cfg, n);
        }
    }

    template <IsConfig Config>
        requires(!needVariable)
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg) const noexcept {

        if constexpr (details::isPermanent<T>()) {
            T::initMemory(mem, end, cfg);
            return maxSize;
        } else {
            return T::generate(mem, end, cfg);
        }
    }
};

}