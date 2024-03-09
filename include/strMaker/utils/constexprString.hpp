#pragma once

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