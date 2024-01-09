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

    template <size_t Size, IsConfig Config>
    void initMemory(Memory<Size>& mem, IndexT pos, Config const& cfg) const noexcept {
        Try::initMemory(mem, pos, cfg);
    }

    template <IndexT memSize, IsConfig Config, typename Arg>
        requires (needVariable)
    IndexT generate(Memory<memSize>& mem, IndexT pos, Config const& cfg, Arg&& arg) const {

        auto newPos = Try::generate(mem, pos, cfg, arg);
        if (newPos != ERROR_INDEX) {
            return newPos;
        } else {
            if constexpr (details::checkNeedVariable<Catch>()) {
                return Catch::generate(mem, pos, cfg, arg);
            } else {
                Catch::initMemory(mem, pos, cfg);
                return pos + details::getSize<Catch>();
            }
        }
    }

    template <IndexT memSize, IsConfig Config, typename Arg>
        requires (!needVariable)
    IndexT generate(Memory<memSize>& mem, IndexT pos, Config const& cfg) const {

        auto newPos = Try::generate(mem, pos, cfg);
        if (newPos != ERROR_INDEX) {
            return newPos;
        } else {
            Catch::initMemory(mem, pos, cfg);
            return pos + details::getSize<Catch>();
        }
    }
};

}