#pragma once

#include "aggregator.hpp"
#include "../patterns/staticString.hpp"
#include "../utils/constexprString.hpp"

namespace mkr {

namespace details {

template <size_t N, ConstexprString<N> str, typename Arg, typename ...Args>
consteval auto split() {
    auto constexpr pos = str.find("{}"_str);
    static_assert (pos != std::string::npos, "Arguments more then braces {}");
    static_assert (pos + 2 <= N);
    if constexpr (sizeof...(Args) == 0) {
        static_assert(str.template substr<pos + 2>().find("{}"_str) == std::string::npos, "Arguments less then braces {}");
        return std::make_tuple(
                StaticStr<str.template substr<0, pos>()>{}
                , Arg{}
                , StaticStr<str.template substr<pos + 2>()>{});
    } else {
        auto tt = std::make_tuple(
                StaticStr<str.template substr<0, pos>()>{}
                , Arg{});
        auto next = split<N - pos - 2, str.template substr<pos + 2>(), Args...>();
        return std::tuple_cat(tt, next);
    }
}

template <typename Skip, typename Arg, typename ...Args>
consteval auto optimizer1(Arg, Args... args) {
    auto const next = ([&]() {
        if constexpr (sizeof...(Args) == 0) {
            return std::tuple();
        } else {
            return optimizer1<Skip>(args...);
        }
    })();

    if constexpr (Skip::template check<Arg>()) {
        return next;
    } else {
        return std::tuple_cat(std::make_tuple<Arg>({}), next);
    }
}

template <typename Arg1, typename Arg2, typename ...Args>
consteval auto optimizer2(Arg1 const&, Arg2 const&, Args const&...args) {
    auto const next = ([&]() {
        if constexpr (sizeof...(Args) >= 2) {
            return optimizer2(args...);
        } else if constexpr (sizeof...(Args) == 1) {
            return std::tuple<details::GetIthType<0, Args...>>();
        } else {
            return std::tuple();
        }
    })();

    if constexpr (details::is_value_template<Arg1, StaticStr> && details::is_value_template<Arg2, StaticStr>) {
        using NewStr = StaticStr<Arg1::str + Arg2::str>;
        return std::tuple_cat(std::make_tuple<NewStr>(), next);
    } else {
        return std::tuple_cat(std::make_tuple<Arg1, Arg2>({}, {}), next);
    }
}

struct StaticStrZeroSize {
    template <class T>
    static consteval bool check() {
        return std::is_same_v<StaticStr<""_str>, T>;
    }
};


template <typename ...Args>
struct Combine : Args... {
    template <class T>
    static constexpr bool check() {
        return (false || ... || Args::template check<T>());
    }
};


template <typename Arg> requires(details::is_template<Arg, std::tuple> && std::tuple_size_v<Arg> >= 1)
using Optimize1 = decltype(std::apply([](auto ...args) {
    return optimizer1<StaticStrZeroSize>(args...);
}, std::declval<Arg>()));


template <typename Arg> requires(details::is_template<Arg, std::tuple> && std::tuple_size_v<Arg> >= 2)
struct Optimize2 {
    using type = decltype(std::apply([](auto&& ...args) {
        if constexpr (sizeof...(args) >= 2) {
            return optimizer2(args...);
        } else {
            return std::declval<details::GetIthType<0, decltype(args)...>>();
        }
    }, std::declval<Arg>()));
};

template <typename T>
struct AggWrapper;

template <IsPattern ...Args>
struct AggWrapper<std::tuple<Args...>> {
    using type = Aggregator<Args...>;
};

template <auto str, IsPattern ...Args>
struct MakeAggregatorImpl {
    static_assert(sizeof...(Args) > 0);
    using Unoptimized = decltype(details::split<str.size(), str, Args...>());

    using Opt1 = Optimize1<Unoptimized>;
    static_assert(std::tuple_size<Opt1>::value > 0, "No one template after opt1");
    using Opt2 = Optimize2<Opt1>::type;
    static_assert(std::tuple_size<Opt2>::value > 0, "No one template after opt2");

    using type = details::AggWrapper<Opt2>::type;
};

template <size_t N, ConstexprString<N> str>
struct MakeAggregatorImpl<str> {
    static_assert(str.find("{}"_str) == std::string::npos, "Arguments less than braces {}");
    using type = Aggregator<StaticStr<str>>;
};


template <auto str, IsPattern ...Args>
using Unoptimized = AggWrapper<typename MakeAggregatorImpl<str, Args...>::Unoptimized>::type;

} // details

template <auto str, IsPattern ...Args>
using MakeAggregator = details::MakeAggregatorImpl<str, Args...>::type;

} // mkr