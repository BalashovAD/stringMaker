#pragma once

#include "pattern.hpp"

namespace mkr {

namespace details {

template <typename T>
struct CustomMaxSize;

template <typename T>
constexpr IndexT customMaxSize() {
    using PureT = std::remove_cvref_t<T>;

    if constexpr (helper::Has_mkr_maxSize<PureT>) {
        return PureT::mkr_maxSize;
    } else {
        return CustomMaxSize<PureT>::maxSize;
    }
}

struct mkr_generate_fn {
    template <IsConfig Cfg, typename T>
    char* operator()(char* begin, char* end, Cfg const& cfg, T&& val) const
            noexcept(noexcept(mkr_generate(begin, end, cfg, std::forward<T>(val)))) {
        return mkr_generate(begin, end, cfg, std::forward<T>(val));
    }
};

template <class T, class U>
concept explicitly_convertible_to = requires(T t) { static_cast<U>(t); };

}

inline constexpr auto mkr_generate = details::mkr_generate_fn{};


template <typename T, IndexT maxSizeSuggestion = 0>
class CustomType {
public:
    static constexpr bool permanent = false;
    static constexpr bool needVariable = true;
    static constexpr IndexT maxSize = maxSizeSuggestion == 0 ? details::customMaxSize<T>() : maxSizeSuggestion;

    template <size_t Size, IsConfig Config>
    void initMemory(Memory<Size>& mem, IndexT pos, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, pos, cfg);
    }

    template <IndexT memSize, IsConfig Config, typename Arg>
    IndexT generate(Memory<memSize>& mem, IndexT pos, Config const& cfg, Arg&& arg) const {

        static_assert(details::explicitly_convertible_to<Arg, T>);

        auto* ptr = mkr_generate(mem.data() + pos, mem.data() + pos + maxSize, cfg, static_cast<T>(arg));
        if (ptr == nullptr) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        } else {
            return ptr - mem.data();
        }
    }
};

}