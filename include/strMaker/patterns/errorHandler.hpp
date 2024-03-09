#pragma once

#include "pattern.hpp"
#include "../utils/template.hpp"

namespace mkr {

template <IsPattern Try, IsPattern Catch>
class ErrorHandler : private Try, private Catch {
public:
    static_assert(!details::isPermanent<Try>());

    static constexpr bool permanent = false;
    static constexpr bool needVariable = details::checkNeedVariable<Try>() || details::checkNeedVariable<Try>();
    static constexpr IndexT maxSize = std::max(details::getSize<Try>(), details::getSize<Catch>());

    template <IsConfig Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        Try::initMemory(mem, end, cfg);
    }

    template <IsConfig Config, typename Arg>
        requires (needVariable)
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg, Arg&& arg) const {

        auto newPos = Try::generate(mem, end, cfg, arg);
        if (newPos != ERROR_INDEX) {
            return newPos;
        } else {
            if constexpr (details::checkNeedVariable<Catch>()) {
                return Catch::generate(mem, end, cfg, arg);
            } else {
                Catch::initMemory(mem, end, cfg);
                return details::getSize<Catch>();
            }
        }
    }

    template <IsConfig Config, typename Arg>
        requires (!needVariable)
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg) const {

        auto newPos = Try::generate(mem, end, cfg);
        if (newPos != ERROR_INDEX) {
            return newPos;
        } else {
            Catch::initMemory(mem, end, cfg);
            return details::getSize<Catch>();
        }
    }
};

}