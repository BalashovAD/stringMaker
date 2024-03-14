#pragma once

#include "storage.hpp"

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
#ifdef __EXCEPTIONS
                throw std::runtime_error("RingBufferStorage overflow");
#else
                exit(1);
#endif
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