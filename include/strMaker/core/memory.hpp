#pragma once

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

static constexpr IndexT ERROR_INDEX = ~0;

}