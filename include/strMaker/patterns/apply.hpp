#pragma once

#include "pattern.hpp"
#include "../utils/template.hpp"


namespace mkr {

template <IsPattern Pattern, auto f>
class Apply : private Pattern {
public:
    static_assert(!details::isPermanent<Pattern>());
    static_assert(details::checkNeedVariable<Pattern>());

    static constexpr auto permanent = false;
    static constexpr auto needVariable = true;
    static constexpr auto maxSize = details::getSize<Pattern>();

    using Pattern::initMemory;

    template <IsConfig Config, typename Arg>
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg, Arg&& arg) const {
        return Pattern::generate(mem, end, cfg, f(arg));
    }
};

}