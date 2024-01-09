#pragma once

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

}