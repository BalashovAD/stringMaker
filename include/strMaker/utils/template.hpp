#pragma once

#include "../core/memory.hpp"

#include <array>
#include <tuple>


namespace mkr::details {

namespace helper {

#define HAS_STATIC_VAR(VarName, VarType) \
template <typename T>\
concept Has_ ## VarName = requires {     \
    requires std::convertible_to<decltype(T::VarName), VarType>; \
}

HAS_STATIC_VAR(maxSize, IndexT);
HAS_STATIC_VAR(mkr_maxSize, IndexT);
HAS_STATIC_VAR(permanent, bool);
HAS_STATIC_VAR(needVariable, bool);
HAS_STATIC_VAR(dynamicLocated, bool);

#undef HAS_STATIC_VAR

}

template <typename T>
constexpr auto getSize() {
    using PureT = std::remove_cvref_t<T>;

    if constexpr (helper::Has_maxSize<PureT>) {
        return PureT::maxSize;
    } else {
        static_assert(false, "Type should have maxSize");
        return 0;
    }
}

template <typename T>
constexpr auto isPermanent() {
    using PureT = std::remove_cvref_t<T>;

    if constexpr (helper::Has_permanent<PureT>) {
        return PureT::permanent;
    } else {
        return false;
    }
}


template <typename T>
constexpr auto checkNeedVariable() {
    using PureT = std::remove_cvref_t<T>;

    if constexpr (helper::Has_needVariable<PureT>) {
        return PureT::needVariable;
    } else {
        return false;
    }
}



template <typename T>
constexpr auto isDynamicLocated() {
    using PureT = std::remove_cvref_t<T>;

    if constexpr (helper::Has_dynamicLocated<PureT>) {
        return PureT::dynamicLocated;
    } else {
        return false;
    }
}


template <typename Arg1, typename ...Args>
constexpr IndexT sumSize() {
    if constexpr (sizeof...(Args) > 0) {
        return getSize<Arg1>() + sumSize<Args...>();
    } else {
        return getSize<Arg1>();
    }
}


struct PatternInfo {
    IndexT pos;
    IndexT size;
    int varInd;
    bool needCall;
    bool dynamicLocated;
};

template <size_t size>
using InfoList = std::array<PatternInfo, size>;

namespace helper {

template <typename T>
constexpr auto makeInfo(IndexT shift, int varShift) {
    IndexT size = getSize<T>();
    bool needVariable = checkNeedVariable<T>();
    bool needCall = !isPermanent<T>();
    bool dynamicLocated = isDynamicLocated<T>();
    return PatternInfo{shift, size, needVariable ? varShift : -1, needCall, dynamicLocated};
}


template <size_t shift, typename Arg1, typename ...Args>
requires(shift < sizeof...(Args) + 1)
struct GetIthTypeT {
    template <typename T>
    struct Id {
        using type = T;
    };

    struct NextStep {
        using type = GetIthTypeT<shift - 1, Args...>::type;
    };

    using type = std::conditional_t<shift == 0, Id<Arg1>, NextStep>::type;
};

}




template <size_t shift, typename ...Args>
using GetIthType = helper::GetIthTypeT<shift, Args...>::type;


template <size_t shift, typename Fn, typename ...Args>
consteval void foreachId(Fn const& f) {
    using T = GetIthType<shift, Args...>;
    f(std::integral_constant<T*, nullptr>{}, std::integral_constant<size_t, shift>{});
    if constexpr (shift + 1 < sizeof...(Args)) {
        foreachId<shift + 1, Fn, Args...>(f);
    }
}



template <size_t shift = 0, typename Fn, typename ...Args>
constexpr void foreachTupleId(Fn const& f, std::tuple<Args...> const& t) {
    f(std::get<shift>(t), std::integral_constant<size_t, shift>{});
    if constexpr (shift + 1 < sizeof...(Args)) {
        foreachTupleId<shift + 1, Fn, Args...>(f, t);
    }
}


template <typename ...Args>
consteval InfoList<sizeof...(Args)> makePatternInfoList() {
    constexpr auto size = sizeof...(Args);
    InfoList<size> list{};
    int posShift = 0;
    int varShift = 0;
    auto const f = [&]<typename T, size_t shift>(std::integral_constant<T*, nullptr>, std::integral_constant<size_t, shift>) {
        list[shift] = helper::makeInfo<T>(posShift, varShift);
        posShift += list[shift].size;
        if (list[shift].varInd != -1) {
            ++varShift;
        }
    };
    foreachId<0, decltype(f), Args...>(f);
    return list;
}

}