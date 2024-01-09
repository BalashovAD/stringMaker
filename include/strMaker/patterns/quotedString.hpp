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

    template <size_t Size, IsConfig Config>
    void initMemory(Memory<Size>& mem, IndexT pos, Config const& cfg) const noexcept {
        mem[pos] = '"';
        Pattern::initMemory(mem, pos + 1, cfg);
        mem[pos + internalMaxSize + 1] = quote;
    }

    template <IndexT memSize, IsConfig Config, typename Arg>
    IndexT generate(Memory<memSize>& mem, IndexT pos, Config const& cfg, Arg&& arg) const {
        mem[pos] = quote;
        auto newPos = Pattern::generate(mem, pos + 1, cfg, std::forward<Arg>(arg));

        if (newPos == ERROR_INDEX) [[unlikely]] {
            return ERROR_INDEX;
        }

        mem[newPos] = quote;
        return newPos + 1;
    }
};

}