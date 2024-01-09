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
        IndexT it = 0;
        details::foreachTupleId([&](auto const& pattern, auto) {
            pattern.initMemory(memory, it, cfg);
            memory.setInitialized(true);
            it += details::getSize<decltype(pattern)>();
        }, m_pattern);

        assert(it == maxSize);
    }


    template <IsConfig Cfg, typename ...Args>
    std::string_view generate(Memory<maxSize>& memory, Cfg const& cfg, Args&&...args) {

        static_assert(validateArgs<sizeof...(Args)>());

        auto argsTuple = std::forward_as_tuple(args...);
        bool hasError = false;
        IndexT pos = 0;

        const bool needInit = needPreInitMemory<Cfg::mode()>(memory.isInitialized());
        details::foreachTupleId([&]<typename T, size_t shift>(T const& pattern, std::integral_constant<size_t, shift>) {
            constexpr details::PatternInfo info = infoList[shift];
            if (hasError) [[unlikely]] {
                return;
            }

            auto currentPos = info.pos;
            if constexpr (info.dynamicLocated) {
                currentPos = pos;
            } else {
                if constexpr (getPreviousInfo<shift>().needCall) {
                    if (needInit) {
                        cfg.memset(memory.data() + pos, cfg.neutralSymbol(), info.pos - pos);
                    }
                }
            }

            if constexpr (info.needCall) {
                if constexpr (info.varInd != -1) {
                    pos = pattern.generate(memory, currentPos, cfg, std::get<info.varInd>(argsTuple));
                } else {
                    pos = pattern.generate(memory, currentPos, cfg);
                }
                hasError = pos == ERROR_INDEX;
            } else {
                pos = info.pos + info.size;
            }
        }, m_pattern);

        memory.setInitialized(false);
        if (hasError) [[unlikely]] {
            return std::string_view{};
        } else {
            return std::string_view{memory.data(), pos};
        }
    }


    template <IsConfig Cfg, typename ...Args>
        requires (Cfg::mode() == AggregatorMode::DYNAMIC)
    std::string_view generate(Memory<maxSize>& memory, Cfg const& cfg, Args&&...args) {

        static_assert(validateArgs<sizeof...(Args)>(), "Count of arguments more or less than expected");

        auto argsTuple = std::forward_as_tuple(args...);
        bool hasError = false;
        IndexT pos = 0;

        details::foreachTupleId([&]<typename T, size_t shift>(T const& pattern, std::integral_constant<size_t, shift>) {
            constexpr details::PatternInfo info = infoList[shift];
            if (hasError) [[unlikely]] {
                return;
            }

            if constexpr (info.needCall) {
                if constexpr (info.varInd != -1) {
                    pos = pattern.generate(memory, pos, cfg, std::get<info.varInd>(argsTuple));
                } else {
                    pos = pattern.generate(memory, pos, cfg);
                }
                hasError = pos == ERROR_INDEX;
            } else {
                pattern.initMemory(memory, pos, cfg);
                pos += info.size;
            }
        }, m_pattern);

        if (hasError) [[unlikely]] {
            return std::string_view{};
        } else {
            return std::string_view{memory.data(), pos};
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