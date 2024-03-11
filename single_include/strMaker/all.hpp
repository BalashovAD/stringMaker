#pragma once

// #include "fwd.hpp"


//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

// #include "strMaker.hpp"


// #include "storage/storage.hpp"


// #include "../core/memory.hpp"


#include <string_view>
#include <array>

namespace mkr {


template <size_t Size>
struct Memory : public std::array<char, Size> {
    Memory() noexcept {
        this->fill('~');
    }

    void setInitialized(bool v) noexcept {
        m_isInitialized = v;
    }

    bool isInitialized() const noexcept {
        return m_isInitialized;
    }
private:
    bool m_isInitialized = false;
};

using IndexT = size_t;
using CharIt = char*;
using CharEnd = char const*const;

static constexpr IndexT ERROR_INDEX = ~0;

}
// #include "../utils/template.hpp"


// #include "../core/memory.hpp"


#include <array>
#include <tuple>


namespace mkr::details {

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<typename T, template<typename...> typename Tmpl>
concept is_template = decltype(overloaded{
        []<typename ...U>(const Tmpl<U...> &) { return std::true_type{}; },
        [](const auto &) { return std::false_type{}; },
}(std::declval<T>()))::value;

template<typename T, template<auto...> typename Tmpl>
concept is_value_template = decltype(overloaded{
        []<auto ...U>(const Tmpl<U...> &) { return std::true_type{}; },
        [](const auto &) { return std::false_type{}; },
}(std::declval<T>()))::value;

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

#include <string_view>
#include <ranges>

namespace mkr {

template <typename T>
concept IsStorage = requires (T t) {
    { t.allMemory() } -> std::ranges::range;
    { t.getMemory() } -> std::same_as<Memory<details::getSize<T>()>&>;
};




}
// #include "storage/localStorage.hpp"


#include <string_view>

namespace mkr {

namespace details {

template<typename T>
class SingleItemIterable {
public:
    explicit SingleItemIterable(T& item) noexcept
        : m_iter(item) {}

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        Iterator() = default;
        explicit Iterator(pointer ptr) : m_ptr(ptr) {}

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }

        Iterator& operator++() {
            m_ptr = nullptr;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

    private:
        pointer m_ptr = nullptr;
    };

    Iterator begin() const { return Iterator(&m_iter); }
    Iterator end() const { return Iterator(nullptr); }

private:
    T& m_iter;
};

}


template <size_t maxSizeParam>
    requires (maxSizeParam > 0)
class LocalStorage {
public:
    static constexpr IndexT maxSize = maxSizeParam;

    constexpr auto allMemory() noexcept {
        return details::SingleItemIterable{m_memory};
    }

    Memory<maxSize>& getMemory() noexcept {
        return m_memory;
    }
private:
    Memory<maxSize> m_memory;
};

namespace details {

template <size_t maxSizeParam>
        requires (maxSizeParam > 0)
class LocalStorageAlign {
public:
    static constexpr IndexT maxSize = maxSizeParam;

    constexpr auto allMemory() noexcept {
        return details::SingleItemIterable{m_memory};
    }

    Memory<maxSize>& getMemory() noexcept {
        return m_memory;
    }
private:
    alignas(16) Memory<maxSize> m_memory;
};

template <size_t maxSizeParam>
    requires (maxSizeParam > 0)
class LocalStorageUnaligned {
public:
    static constexpr IndexT maxSize = maxSizeParam;

    constexpr auto allMemory() noexcept {
        return details::SingleItemIterable{m_memory};
    }

    Memory<maxSize>& getMemory() noexcept {
        return m_memory;
    }
private:
    Memory<maxSize> m_memory;
};

}



}
// #include "core/memory.hpp"

// #include "core/config.hpp"


// #include "../utils/constexprString.hpp"


namespace mkr {

namespace details {

template<std::size_t N>
struct MakeArray
{
    std::array<char, N> data;

    template <std::size_t... Is>
    constexpr MakeArray(char const(&arr)[N], std::integer_sequence<std::size_t, Is...>)
            : data{arr[Is]...} {

    }

    constexpr MakeArray(char const(&arr)[N])
            : MakeArray(arr, std::make_integer_sequence<std::size_t, N>()) {

    }

    constexpr auto size() const {
        return N;
    }
};

}

template<std::size_t N = 0>
struct ConstexprString {
    std::array<char, N + 1> m_str;

    template<std::size_t M>
    constexpr explicit ConstexprString(char const(&s)[M]) noexcept {
        static_assert(M >= N);
        for (std::size_t i = 0; i <= N; ++i) {
            m_str[i] = s[i];
        }
    }

    constexpr explicit ConstexprString(std::array<char, N + 1> arr) noexcept
        : m_str(arr) {
    }

    constexpr explicit ConstexprString(details::MakeArray<N + 1> arr) noexcept{
        for (size_t i = 0; i != N + 1; ++i) {
            m_str[i] = arr.data[i];
        }
    }


    static constexpr ConstexprString<1> fromChar(char c) noexcept {
        return ConstexprString<1>(std::array<char, 2>({c, '0'}));
    }

    template <std::size_t Q>
    constexpr auto repeat() const noexcept {
        constexpr std::size_t NewSize = N * Q;
        std::array<char, NewSize + 1> repeated_str{};

        for (std::size_t q = 0; q < Q; ++q) {
            for (std::size_t i = 0; i < N; ++i) {
                repeated_str[q * N + i] = m_str[i];
            }
        }
        repeated_str[NewSize] = '\0';

        return ConstexprString<NewSize>(repeated_str);
    }

    constexpr const char* c_str() const noexcept {
        return m_str.data();
    }

    constexpr size_t size() const noexcept {
        return N;
    }

    constexpr std::string_view sv() const noexcept {
        return std::string_view(c_str(), size());
    }

    std::string toString() const noexcept {
        return std::string(c_str(), size());
    }

    template<std::size_t M>
    constexpr auto operator+(ConstexprString<M> const& other) const noexcept {
        std::array<char, N + M + 1> new_str{};
        for (std::size_t i = 0; i != N; ++i) {
            new_str[i] = m_str[i];
        }
        for (std::size_t i = 0; i != M; ++i) {
            new_str[N + i] = other.m_str[i];
        }
        return ConstexprString<N + M>(new_str);
    }

    constexpr auto add(char c) const noexcept {
        return operator+(fromChar(c));
    }

    constexpr auto between(char c) const noexcept {
        return fromChar(c) + *this + fromChar(c);
    }


    constexpr auto between(char l, char r) const noexcept {
        return fromChar(l) + *this + fromChar(r);
    }


    template <size_t M>
    constexpr bool operator==(ConstexprString<M> const& rhs) const noexcept {
        if constexpr (M != N) {
            return false;
        } else {
            for (auto i = 0; i != N; ++i) {
                if (rhs[i] != this->operator[](i)) {
                    return false;
                }
            }
            return true;
        }
    }

    template <size_t M>
    constexpr bool operator!=(ConstexprString<M> const& rhs) const noexcept {
        return !operator==(rhs);
    }

    constexpr auto operator[](size_t i) const noexcept {
        return m_str[i];
    }


    template <size_t START, size_t SIZE>
        requires (START + SIZE <= N)
    consteval auto substr() const noexcept {
        constexpr auto T = SIZE;
        std::array<char, T + 1> out{};
        for (auto i = 0; i != T; ++i) {
            out[i] = operator[](START + i);
        }
        out[T] = '\0';
        return ConstexprString<T>(out);
    }


    template <size_t START> requires(START <= N)
    consteval ConstexprString<N - START> substr() const noexcept {
        return substr<START, N - START>();
    }

    template <size_t M>
    requires(M <= N)
    constexpr bool startsFrom(ConstexprString<M> const& prefix) const noexcept {
        return substr<M>() == prefix;
    }

    template <size_t M>
        requires (M > 0)
    consteval size_t find(ConstexprString<M> ss) const noexcept {
        if constexpr (N < M) {
            return std::string::npos;
        } else {
            auto it = m_str.begin();
            auto const end = m_str.end();
            while (it != end) {
                if (*it == *ss.m_str.begin()) {
                    auto i = 0u;
                    while (i != M && *(it + i) == ss[i])
                        ++i;
                    if (i == M) {
                        return it - m_str.begin();
                    }
                }
                ++it;
            }
            return std::string::npos;
        }
    }
};

template <details::MakeArray arr>
constexpr auto operator""_str() {
    return ConstexprString<arr.size() - 1>(arr);
}

template <size_t Q>
constexpr auto repeatStr(char c) {
    return ConstexprString<>::fromChar(c).repeat<Q>();
}


}

#include <cassert>
#include <source_location>
#include <iostream>
#include <cstring>


namespace mkr {

enum class AggregatorMode {
    DEFAULT = 0,
    PRE_INIT = 1,
    PRE_INIT_ONLY = 2,
    DYNAMIC = 3,
};

template <char neutral = ' ', char error = '~', AggregatorMode modeValue = AggregatorMode::DEFAULT>
struct Config {
    constexpr Config() = default;

    static constexpr char neutralSymbol() noexcept {
        return neutral;
    }

    static constexpr char errorSymbol() noexcept {
        return error;
    }

    static constexpr AggregatorMode mode() noexcept {
        return modeValue;
    }

    void errorEvent() const noexcept {

    }

    template <size_t N>
    void fill(char* __restrict__ ptr, ConstexprString<N> const& str) const noexcept {
        memcpy(ptr, str.c_str(), N);
    }

    void memset(char* ptr, char c, size_t size) const {
        ::memset(ptr, c, size);
    }

    void memcpy(char* __restrict__ dst, char const*const __restrict__ src, size_t size) const {
        ::memcpy(dst, src, size);
    }
};


template<typename T>
concept IsConfig = requires(T a, char* ptr, const char* const src, size_t size) {
    { a.neutralSymbol() } -> std::same_as<char>;
    { a.errorSymbol() } -> std::same_as<char>;
    { a.mode() } -> std::same_as<AggregatorMode>;
    { a.errorEvent() };
    { a.fill(ptr, ConstexprString<>::fromChar('a')) };
    { a.memset(ptr, 'a', size) };
    { a.memcpy(ptr, src, size) };
};

template <char neutral = ' ', char error = '~', AggregatorMode mode = AggregatorMode::DEFAULT>
struct DebugConfig : public Config<neutral, error, mode> {
    void errorEvent(std::source_location const& location = std::source_location::current()) const {
        std::cout << "Error: " << location.file_name() << ":" << location.line()
            << " [" << location.function_name() << "]" << std::endl;
    }
};


using DefaultConfig = Config<>;
using DynamicConfig = Config<' ', '~', AggregatorMode::DYNAMIC>;
using PreInitConfig = Config<' ', '~', AggregatorMode::PRE_INIT>;
using PreInitOnlyConfig = Config<' ', '~', AggregatorMode::PRE_INIT_ONLY>;

}

namespace mkr {


template <typename Aggregator, template<size_t maxMem> typename Storage = LocalStorage, IsConfig ConfigParam = DefaultConfig>
class Maker final : public Storage<details::getSize<Aggregator>()>, private ConfigParam {
public:
    using Config = ConfigParam;

    static_assert(IsStorage<Storage<details::getSize<Aggregator>()>>);

    template <typename ...Args>
    constexpr explicit Maker(Args&& ...args)
        : Config(std::forward<Args>(args)...) {

        init();
    }

    void init() {
        for (auto& memory : this->allMemory()) {
            m_pattern.initMemory(memory, config());
            memory.setInitialized(true);
        }
    }

    template <typename ...Args>
    std::string_view generate(Args&& ...args) {
        return m_pattern.generate(this->getMemory(), config(), std::forward<Args>(args)...);
    }
private:
    Config const& config() const noexcept {
        return static_cast<Config const&>(*this);
    }

    Aggregator m_pattern{};
};

}
// #include "core/makeAggregator.hpp"


// #include "aggregator.hpp"


// #include "config.hpp"

// #include "memory.hpp"

// #include "../utils/template.hpp"

// #include "../patterns/pattern.hpp"


// #include "../core/memory.hpp"

// #include "../core/config.hpp"


namespace mkr {

template <typename T>
concept IsPattern = requires {
    {details::getSize<T>()} -> std::convertible_to<size_t>;
    {details::isPermanent<T>()} -> std::convertible_to<bool>;
    {details::checkNeedVariable<T>()} -> std::convertible_to<bool>;
};


namespace details {

template <IndexT size, IsConfig Cfg>
void emptyInitMemory(CharIt mem, CharEnd end, Cfg const& cfg) noexcept {
    cfg.memset(mem, cfg.neutralSymbol(), size);
}

}

}

#include <tuple>
#include <numeric>
#include <string_view>

namespace mkr {

template <IsPattern ...StrPattern>
class Aggregator {
public:
    static constexpr IndexT maxSize = details::sumSize<StrPattern...>();
    static constexpr size_t patternSize = sizeof...(StrPattern);
    static constexpr auto infoList = details::makePatternInfoList<StrPattern...>();

    static_assert(patternSize > 0);

    template <IsConfig Cfg>
    void initMemory(Memory<maxSize>& memory, Cfg const& cfg) {
        auto const end = memory.end();
        auto const begin = memory.begin();
        CharIt it = begin;
        details::foreachTupleId([&](auto const& pattern, auto) {
            pattern.initMemory(it, end, cfg);
            it += details::getSize<decltype(pattern)>();
        }, m_pattern);

        assert(std::distance(begin, it) == maxSize);
    }


    template <IsConfig Cfg, typename ...Args>
    std::string_view generate(Memory<maxSize>& memory, Cfg const& cfg, Args&&...args) {

        static_assert(validateArgs<sizeof...(Args)>());

        auto const begin = memory.begin();
        auto const end = memory.end();
        CharIt it = begin;

        auto argsTuple = std::forward_as_tuple(args...);
        bool hasError = false;

        const bool needInit = needPreInitMemory<Cfg::mode()>(memory.isInitialized());
        details::foreachTupleId([&]<typename T, size_t shift>(T const& pattern, std::integral_constant<size_t, shift>) {
            constexpr details::PatternInfo info = infoList[shift];

            if (hasError) [[unlikely]] {
                return;
            }

            CharIt currentPos;
            if constexpr (info.dynamicLocated) {
                currentPos = it;
            } else {
                currentPos = begin + info.pos;
                if constexpr (getPreviousInfo<shift>().needCall) {
                    if (needInit) {
                        cfg.memset(it, cfg.neutralSymbol(), info.pos - std::distance(begin, it));
                    }
                }
            }

            IndexT diff = 0;
            if constexpr (info.needCall) {
                if constexpr (info.varInd != -1) {
                    diff = pattern.generate(currentPos, end, cfg, std::get<info.varInd>(argsTuple));
                } else {
                    diff = pattern.generate(currentPos, end, cfg);
                }
                hasError = diff == ERROR_INDEX;
                if (hasError) [[unlikely]] {
                    return;
                }
                it = currentPos + diff;
            } else {
                it = begin + info.pos + info.size;
            }
        }, m_pattern);

        memory.setInitialized(false);
        if (hasError) [[unlikely]] {
            return std::string_view{};
        } else {
            return std::string_view{begin, it};
        }
    }


    template <IsConfig Cfg, typename ...Args>
        requires (Cfg::mode() == AggregatorMode::DYNAMIC)
    std::string_view generate(Memory<maxSize>& memory, Cfg const& cfg, Args&&...args) {

        static_assert(validateArgs<sizeof...(Args)>(), "Count of arguments more or less than expected");

        auto argsTuple = std::forward_as_tuple(args...);
        bool hasError = false;

        auto const begin = memory.begin();
        auto const end = memory.end();
        CharIt it = begin;
        // `it` is always `currentPos` for dynamic located

        details::foreachTupleId([&]<typename T, size_t shift>(T const& pattern, std::integral_constant<size_t, shift>) {
            constexpr details::PatternInfo info = infoList[shift];

            if (hasError) [[unlikely]] {
                return;
            }

            IndexT diff = 0;
            if constexpr (info.needCall) {
                if constexpr (info.varInd != -1) {
                    diff = pattern.generate(it, end, cfg, std::get<info.varInd>(argsTuple));
                } else {
                    diff = pattern.generate(it, end, cfg);
                }
                hasError = diff == ERROR_INDEX;
                if (hasError) [[unlikely]] {
                    return;
                }
                it += diff;
            } else {
                pattern.initMemory(it, end, cfg);
                it += info.size;
            }
        }, m_pattern);

        if (hasError) [[unlikely]] {
            return std::string_view{};
        } else {
            return std::string_view{begin, it};
        }
    }
private:
    template <AggregatorMode mode>
    static bool needPreInitMemory(bool isInitialized) noexcept  {
        switch (mode) {
            case AggregatorMode::DEFAULT:
                return true;
            case AggregatorMode::PRE_INIT:
                return !isInitialized;
            case AggregatorMode::PRE_INIT_ONLY:
                assert(isInitialized);
                return false;
            case AggregatorMode::DYNAMIC:
                return false;
        }
        __builtin_unreachable();
    }

    template <size_t argsSize>
    consteval static bool validateArgs() noexcept {

        return std::accumulate(infoList.begin(), infoList.end(), -1, [](int m, details::PatternInfo info) {
            return std::max<int>(m, info.varInd);
        }) + 1 == argsSize;
    }


    static constexpr details::PatternInfo EMPTY_PATTERN_INFO = {0, 0, -1, false, false};

    template <size_t shift>
    constexpr static details::PatternInfo getPreviousInfo() noexcept {
        if constexpr (shift == 0) {
            return EMPTY_PATTERN_INFO;
        } else {
            return infoList[shift - 1];
        }
    }

    std::tuple<StrPattern...> m_pattern;
};

}
// #include "../patterns/staticString.hpp"


// #include "../core/memory.hpp"

// #include "../utils/constexprString.hpp"


namespace mkr {

template <ConstexprString str>
class StaticStr {
public:
    static constexpr bool permanent = true;
    static constexpr IndexT maxSize = str.size();

    template <typename Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        cfg.fill(mem, str);
    }
};


}
// #include "../utils/constexprString.hpp"


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

//#pragma GCC diagnostic pop

// #include "utils/constexprString.hpp"


// #include "patterns/runtimeString.hpp"


// #include "../core/memory.hpp"

// #include "../utils/constexprString.hpp"

// #include "../core/config.hpp"


namespace mkr {

template <IndexT maxSizeParam>
class RuntimeStr {
public:
    static constexpr bool permanent = false;
    static constexpr bool needVariable = true;
    static constexpr IndexT maxSize = maxSizeParam;

    template <IsConfig Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, end, cfg);
    }

    template <IsConfig Config>
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg, std::string_view s) const noexcept {
        if (s.size() > maxSize) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }
        cfg.memcpy(mem, s.data(), s.size());
        return s.size();
    }
};


}
// #include "patterns/quotedString.hpp"


// #include "../core/memory.hpp"

// #include "../utils/template.hpp"


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
// #include "patterns/staticString.hpp"

// #include "patterns/runtimeIntegral.hpp"


// #include "../core/memory.hpp"

// #include "../utils/template.hpp"

// #include "../utils/numberHelper.hpp"


namespace mkr::details {

template <std::integral T>
constexpr size_t maxNumberSize() {
    constexpr bool is_signed = std::is_signed_v<T>;
    constexpr size_t max_digits = std::numeric_limits<T>::digits10 + 1;

    return max_digits + is_signed;
}

template <std::floating_point T>
constexpr size_t maxNumberSize() {
    constexpr size_t maxDigitsBeforePoint = std::numeric_limits<T>::digits10 + 1;

    // Sign (1) + Digits before point (maxDigitsBeforePoint) + Decimal point (1)
    return 1 + maxDigitsBeforePoint + 1;
}

constexpr inline unsigned roundUpTo8(unsigned num) {
    return (num + 7) & ~7u;
}

constexpr unsigned smallestPowOf2GreaterThanN(unsigned n) {
    unsigned result = 1;
    while (result <= n) {
        result <<= 1;
    }
    return result;
}
}

#include <charconv>
#include <concepts>

namespace mkr {

template <std::integral Number, IndexT maxSizeSuggestion = 0>
class RuntimeIntegral {
public:
    static constexpr bool permanent = false;
    static constexpr bool needVariable = true;
    static constexpr IndexT maxSize = maxSizeSuggestion == 0 ? details::maxNumberSize<Number>() : maxSizeSuggestion;

    template <IsConfig Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, end, cfg);
    }

    template <IsConfig Config>
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg, Number n) const noexcept {

        auto result = std::to_chars(mem, mem + maxSize, n);
        if (result.ec != std::errc{}) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }
        return result.ptr - mem;
    }
};

}
// #include "patterns/runtimeFloat.hpp"


// #include "../core/memory.hpp"

// #include "../utils/template.hpp"

// #include "../utils/numberHelper.hpp"


#include <charconv>
#include <cmath>


namespace mkr {

template <std::floating_point Number = double, size_t maxPossibleSize = 0>
class RuntimeFloat {
public:
    static constexpr bool permanent = false;
    static constexpr bool needVariable = true;
    static constexpr IndexT maxSize = maxPossibleSize == 0 ? details::maxNumberSize<Number>() : maxPossibleSize;

    template <IsConfig Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, end, cfg);
    }

    template <IsConfig Config>
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg, Number n) const noexcept {

        if (std::isnan(n)) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }

        auto result = std::to_chars(mem, mem + maxSize, n);
        if (result.ec != std::errc{}) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        }
        return result.ptr - mem;
    }
};


}
// #include "patterns/customType.hpp"


// #include "pattern.hpp"


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

    template <IsConfig Config>
    void initMemory(CharIt mem, CharEnd end, Config const& cfg) const noexcept {
        details::emptyInitMemory<maxSize>(mem, end, cfg);
    }

    template <IsConfig Config, typename Arg>
    IndexT generate(CharIt mem, CharEnd end, Config const& cfg, Arg&& arg) const {

        static_assert(details::explicitly_convertible_to<Arg, T>);

        auto* ptr = mkr_generate(mem, mem + maxSize, cfg, static_cast<T>(arg));
        if (ptr == nullptr) [[unlikely]] {
            cfg.errorEvent();
            return ERROR_INDEX;
        } else {
            return ptr - mem;
        }
    }
};

}
// #include "patterns/dynamicLocated.hpp"


// #include "pattern.hpp"


// #include "../utils/template.hpp"


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
// #include "patterns/errorHandler.hpp"


// #include "pattern.hpp"

// #include "../utils/template.hpp"


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
// #include "patterns/apply.hpp"


// #include "pattern.hpp"

// #include "../utils/template.hpp"



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

// #include "storage/localStorage.hpp"

// #include "storage/ringBufferStorage.hpp"


// #include "storage.hpp"


#include <array>

namespace mkr {

template<IndexT ringSize, bool strictCheck, IndexT maxSizeParam> requires (ringSize > 1)
class RingBufferStorage {
public:
    static constexpr IndexT maxSize = maxSizeParam;

    std::array<Memory<maxSize>, ringSize>& allMemory() noexcept {
        return m_ringBuffer;
    }

    void releaseMemory() noexcept {
        assert(m_usedSlots > 0);
        --m_usedSlots;
    }

    Memory<maxSize>& getMemory() noexcept(!strictCheck) {
        if constexpr (strictCheck) {
            if (m_usedSlots == ringSize) [[unlikely]] {
                throw std::runtime_error("RingBufferStorage overflow");
            }
            ++m_usedSlots;
        }
        m_pos = (m_pos + 1) % ringSize;
        return m_ringBuffer[m_pos];
    }
private:
    std::array<Memory<maxSize>, ringSize> m_ringBuffer;
    unsigned m_pos = ringSize - 1;
    unsigned m_usedSlots = 0;
};


template <IndexT ringSize, bool strictCheck>
struct RingBufferStorageWrapper {
    template <IndexT maxSize>
    using type = RingBufferStorage<ringSize, strictCheck, maxSize>;
};

}