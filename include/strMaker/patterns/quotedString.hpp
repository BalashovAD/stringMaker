#pragma once

#include "../core/memory.hpp"
#include "../utils/template.hpp"

namespace mkr {


template <typename Pattern, char quote = '"'>
class QuotedStr : private Pattern {
public:
    static constexpr IndexT internalMaxSize = details::getSize<Pattern>();
    static constexpr IndexT maxSize = internalMaxSize + 2;
    static constexpr auto permanent = details::isPermanent<Pattern>();
    static constexpr auto needVariable = details::checkNeedVariable<Pattern>();

    template <IsConfig Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {

        mem[0] = quote;
        Pattern::initMemory(mem + 1, end, cfg);
        mem[internalMaxSize + 1] = quote;
    }

    template <IsConfig Config, typename Arg>
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg, Arg&& arg) const {

        mem[0] = quote;
        auto diff = Pattern::generate(mem + 1, end, cfg, std::forward<Arg>(arg));

        if (diff == ERROR_INDEX) [[unlikely]] {
            return ERROR_INDEX;
        }

        mem[diff + 1] = quote;
        return diff + 2;
    }
};

}