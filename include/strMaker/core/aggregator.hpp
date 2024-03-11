#pragma once

#include "config.hpp"
#include "memory.hpp"
#include "../utils/template.hpp"
#include "../patterns/pattern.hpp"

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