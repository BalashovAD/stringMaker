#include "helper.hpp"

#include <chrono>
#include <iomanip>
#include <ctime>
#include <ostream>
#include <streambuf>
#include <string>
#include <sstream>

// 2024-01-04 23:36:51.358;6A1BF05990AAC001;sip:from@organization.org;sip:to@organization.org;OUT;192.168.200.152:5060;192.168.250.11:5060;f_169eac17a017b0a4e0adfa8_I;1704387681019;162;162;B;200

enum class Source {
    IN,
    OUT,
};

template<>
struct details::CustomMaxSize<Source> {
    static constexpr IndexT maxSize = 3;
};

template <IsConfig Cfg>
inline char* mkr_generate(char* begin, [[maybe_unused]] char* end, [[maybe_unused]] Cfg const& cfg, Source const& t) {
    if (t == Source::IN) {
        begin[0] = 'I';
        begin[1] = 'N';
        return begin + 2;
    } else {
        begin[0] = 'O';
        begin[1] = 'U';
        begin[2] = 'T';
        return begin + 3;
    }
}


struct HP {
    std::string host;
    unsigned port;

    static constexpr IndexT mkr_maxSize = 32 + 1 + 5;

    template <IsConfig Cfg>
    friend char* mkr_generate(char* begin, char* end, Cfg const& cfg, HP const& t) noexcept {
        if (t.host.size() > 32) {
            return nullptr;
        }
        cfg.memcpy(begin, t.host.data(), t.host.size());
        if (t.port != 0) {
            *(begin + t.host.size()) = ':';
            auto ec = std::to_chars(begin + t.host.size() + 1, end, t.port);
            return ec.ptr;
        } else {
            return begin + t.host.size();
        }
    }

    friend std::ostream& operator<<(std::ostream& os, HP const& t) noexcept {
        os << t.host;
        if (t.port != 0) {
            os << ':' << t.port;
        }
        return os;
    }
};

template<>
struct std::formatter<HP> : std::formatter<std::string> {
    // Formats a TimeFormat object
    template<typename FormatContext>
    auto format(const HP& t, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << t.host;
        if (t.port != 0) {
            oss << ':' << t.port;
        }
        return std::formatter<std::string>::format(oss.str(), ctx);
    }
};

struct TimeFormat {
    std::chrono::system_clock::time_point tp;

    static constexpr IndexT mkr_maxSize = 23;

    template <IsConfig Cfg>
    friend char* mkr_generate(char* begin, char* end, Cfg const& cfg, TimeFormat const& t) noexcept {
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                t.tp.time_since_epoch()) % 1000;
        const auto timeT = std::chrono::system_clock::to_time_t(t.tp);

        tm *localTime = std::localtime(&timeT); // internal static object

        auto size = std::strftime(begin, end - begin, "%Y-%m-%d %H:%M:%S", localTime);
        cfg.fill(begin + size, "."_str);
        auto ec = std::to_chars(begin + size + 1, end, ms.count());
        return ec.ptr;
    }

    friend std::ostream& operator<<(std::ostream& os, TimeFormat const& t) noexcept {
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                t.tp.time_since_epoch()) % 1000;
        const auto timeT = std::chrono::system_clock::to_time_t(t.tp);

        tm *localTime = std::localtime(&timeT); // internal static object

        os << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        os << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return os;
    }
};

template<>
struct std::formatter<TimeFormat> : std::formatter<std::string> {
    // Formats a TimeFormat object
    template<typename FormatContext>
    auto format(const TimeFormat& t, FormatContext& ctx) const {
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                t.tp.time_since_epoch()) % 1000;
        const auto timeT = std::chrono::system_clock::to_time_t(t.tp);

        tm *localTime = std::localtime(&timeT); // internal static object

        std::ostringstream oss;
        oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

        return std::formatter<std::string>::format(oss.str(), ctx);
    }
};

using CDR = std::tuple<TimeFormat, std::string, std::string, std::string
, Source, HP, HP, std::string, long long, unsigned, unsigned, std::string, std::string>;

std::vector<CDR> generateCases(unsigned size) {
    using namespace std::chrono;
    std::vector<CDR> out; out.reserve(size);
    for (auto i = 0u; i != size; ++i) {
        auto tp = std::chrono::system_clock::now() + milliseconds(getRandomNumber(-10000, 10000));
        std::string id = generateRandomString(16);
        std::string from = "sip:" + generateRandomString(getRandomNumber(5, 15)) + "@organization.org";
        std::string to = "sip:" + generateRandomString(getRandomNumber(5, 15)) + "@organization.org";
        Source source = getRandomNumber(0, 1) == 0 ? Source::IN : Source::OUT;
        HP l{generateRandomString(15), getRandomNumber(1000, 55000)};
        HP r{generateRandomString(15), getRandomNumber(1000, 55000)};
        std::string callId = generateRandomString(getRandomNumber(15, 63));
        auto start = duration_cast<milliseconds>((system_clock::now() + milliseconds(getRandomNumber(-10000, 10000))).time_since_epoch()).count();
        auto d = getRandomNumber(0, 3600);
        std::string side = "B";
        std::string code = std::to_string(getRandomNumber(200, 600));
        CDR cdr{TimeFormat{tp}, id, from, to, source, l, r, callId, start, d, d, side, code};
        out.emplace_back(cdr);
    }
    return out;
}

#define TUPLE_ARGS get<0>(args), get<1>(args), get<2>(args), get<3>(args), get<4>(args), get<5>(args), get<6>(args), get<7>(args), get<8>(args), get<9>(args), get<10>(args), get<11>(args), get<12>(args)
#define TUPLE_ARGS_ENUM_CAST get<0>(args), get<1>(args), get<2>(args), get<3>(args), (get<4>(args) == Source::IN ? "IN" : "OUT"), get<5>(args), get<6>(args), get<7>(args), get<8>(args), get<9>(args), get<10>(args), get<11>(args), get<12>(args)
#define TUPLE_ARGS_STREAM get<0>(args) << ";" << get<1>(args) << ";" << get<2>(args) << ";" << get<3>(args) << ";" << (get<4>(args) == Source::IN ? "IN" : "OUT") << ";" << get<5>(args) << ";" << get<6>(args) << ";" << get<7>(args) << ";" << get<8>(args) << ";" << get<9>(args) << ";" << get<10>(args) << ";" << get<11>(args) << ";" << get<12>(args)

static inline auto BENCH_CASES = generateCases(TEST_SIZE);

template <typename Maker>
static void BM_CDR(benchmark::State& state, Maker maker) {

    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {
            auto const& args = BENCH_CASES[i];
            auto sv = maker.generate(TUPLE_ARGS);
            if (sv.empty()) [[unlikely]] {
                state.SkipWithError("EMPTY");
            }
            benchmark::DoNotOptimize(sv);
        }
    }
    state.SetItemsProcessed(TEST_SIZE * state.iterations());
}


static void BM_mkrCDR_formatCDR(benchmark::State& state) {
    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {
            auto const& args = BENCH_CASES[i];
            auto str = std::format("{};{};{};{};{};{};{};{};{};{};", get<0>(args), get<1>(args), get<2>(args), get<3>(args), "OUT", get<5>(args), get<6>(args), get<7>(args), get<8>(args), get<9>(args), get<10>(args), get<11>(args), get<12>(args));
            benchmark::DoNotOptimize(str);
        }
    }
    state.SetItemsProcessed(TEST_SIZE * state.iterations());
}

static void BM_mkrCDR_streamCDR(benchmark::State& state) {
    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {
            auto const& args = BENCH_CASES[i];
            std::ostringstream oss;
            oss << TUPLE_ARGS_STREAM;
            auto str = oss.str();
            benchmark::DoNotOptimize(str);
        }
    }
    state.SetItemsProcessed(TEST_SIZE * state.iterations());
}

// No checks at all in this code
static void BM_mkrCDR_perfectCDR(benchmark::State& state) {
    // Maker::SIZE ~= 350
    DefaultConfig cfg{};

    for (auto _ : state) {
        for (auto i = 0; i != TEST_SIZE; ++i) {
            char buffer[350];
            const auto end = buffer + 350;
            auto* ptr = buffer;

            auto const addDelimiter = [&]() {
                static constexpr auto delimiter = ";"_str;
                cfg.fill(ptr, delimiter); ++ptr;
            };
            auto const addStr = [&](auto const& str) {
                cfg.memcpy(ptr, str.c_str(), str.size());
                ptr += str.size();
                addDelimiter();
            };
            auto const addMkr = [&](auto const& t) {
                ptr = mkr::mkr_generate(ptr, end, cfg, t);
                addDelimiter();
            };
            auto const addNumber = [&](auto const& t) {
                ptr = std::to_chars(ptr, end, t).ptr;
                addDelimiter();
            };

            auto const& args = BENCH_CASES[i];
            addMkr(get<0>(args));
            addStr(get<1>(args));
            addStr(get<2>(args));
            addStr(get<3>(args));
            addMkr(get<4>(args));
            addMkr(get<5>(args));
            addMkr(get<6>(args));
            addStr(get<7>(args));
            addNumber(get<8>(args));
            addNumber(get<9>(args));
            addNumber(get<10>(args));
            addStr(get<11>(args));
            addStr(get<12>(args));

            benchmark::DoNotOptimize(buffer);
        }
    }

    state.SetItemsProcessed(TEST_SIZE * state.iterations());
}

using Aggr = Aggregator<
        CustomType<TimeFormat>, StaticStr<";"_str>, RuntimeStr<16>, StaticStr<";"_str>, RuntimeStr<64>, StaticStr<";"_str>, RuntimeStr<64>, StaticStr<";"_str>,
        CustomType<Source>, StaticStr<";"_str>, CustomType<HP>, StaticStr<";"_str>, CustomType<HP>, StaticStr<";"_str>, RuntimeStr<64>,
        RuntimeIntegral<long long, 13>, StaticStr<";"_str>, RuntimeIntegral<unsigned, 5>, StaticStr<";"_str>, RuntimeIntegral<unsigned, 5>,
        RuntimeStr<1>, StaticStr<";"_str>, RuntimeStr<3>>;

BENCHMARK_CAPTURE(BM_CDR, Dynamic, Maker<Aggr, LocalStorage, DynamicConfig>());
BENCHMARK_CAPTURE(BM_CDR, RingDynamic, Maker<Aggr, RingStorage::type, DynamicConfig>());
BENCHMARK_CAPTURE(BM_CDR, Default, Maker<Aggr, LocalStorage, DefaultConfig>());
BENCHMARK_CAPTURE(BM_CDR, RingPreInitOnly, Maker<Aggr, RingStorage::type, PreInitOnlyConfig>());
BENCHMARK(BM_mkrCDR_perfectCDR)->Name("BM_CDR/Perfect");

BENCHMARK(BM_mkrCDR_formatCDR)->Name("BM_CDR/Format");
BENCHMARK(BM_mkrCDR_streamCDR)->Name("BM_CDR/Stream");
