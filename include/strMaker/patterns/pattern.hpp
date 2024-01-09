#pragma once

#include "../core/memory.hpp"
#include "../core/config.hpp"

namespace mkr {


template <typename T>
concept IsPattern = true;


namespace details {

template <IndexT size, IndexT maxSize, IsConfig Cfg>
void emptyInitMemory(Memory<maxSize>& memory, IndexT pos, Cfg const& cfg) noexcept {
    cfg.fill(memory.data() + pos, repeatStr<size>(cfg.neutralSymbol()));
}

}

}