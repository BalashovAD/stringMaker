#pragma once

#include "../core/memory.hpp"
#include "../core/config.hpp"

namespace mkr {

template <typename T>
concept IsPattern = requires {
    {details::getSize<T>()} -> std::convertible_to<size_t>;
    {details::isPermanent<T>()} -> std::convertible_to<bool>;
    {details::checkNeedVariable<T>()} -> std::convertible_to<bool>;
};


namespace details {

template <IndexT size, IsConfig Cfg>
void emptyInitMemory(CharIt mem, [[maybe_unused]] CharEnd end, Cfg const& cfg) noexcept {
    cfg.memset(mem, cfg.neutralSymbol(), size);
}

}

}