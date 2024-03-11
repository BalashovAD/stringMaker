#pragma once

#include "../include/strMaker/all.hpp"

#include <benchmark/benchmark.h>

#include <format>
#include <random>
#include <string_view>

using namespace mkr;

static constexpr int TEST_SIZE = 1000;

template <typename Maker, typename ...Args>
void runBenchBatch(benchmark::State& state, Maker& maker, Args const& ...args) {
    if constexpr (Maker::Config::mode() == AggregatorMode::PRE_INIT_ONLY
                  || Maker::Config::mode() == AggregatorMode::PRE_INIT) {
        state.PauseTiming();
        maker.init();
        state.ResumeTiming();
    }

    std::string_view sv;
    for (auto i = 0; i != TEST_SIZE; ++i) {
        sv = maker.generate(args...);
        benchmark::DoNotOptimize(sv);
    }
}


template <typename Maker, typename ...Args>
void testMaker(benchmark::State& state, Maker& maker, Args const& ...args) {

    std::string_view sv;
    sv = maker.generate(args...);
    if (sv.empty()) {
        state.SkipWithError("Error in maker");
    }

    if constexpr (Maker::Config::mode() == AggregatorMode::PRE_INIT_ONLY
                  || Maker::Config::mode() == AggregatorMode::PRE_INIT) {
        maker.init();
    }
}

using RingStorage = RingBufferStorageWrapper<TEST_SIZE, false>;

inline unsigned getRandomNumber(unsigned l, unsigned r) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned> distrib(l, r);
    return distrib(gen);
}

inline std::string generateRandomString(size_t length) {
    const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

    std::string random_string;
    for (size_t i = 0; i != length; ++i) {
        random_string += characters[getRandomNumber(0, characters.size() - 1)];
    }

    return random_string;
}
