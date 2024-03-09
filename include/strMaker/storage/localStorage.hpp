#pragma once

#include <string_view>

#include "../utils/numberHelper.hpp"

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