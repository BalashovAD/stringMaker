#pragma once

#include "../core/memory.hpp"
#include "../utils/template.hpp"

#include <string_view>
#include <ranges>

namespace mkr {

template <typename T>
concept IsStorage = requires (T t) {
    { t.allMemory() } -> std::ranges::range;
    { t.getMemory() } -> std::same_as<Memory<details::getSize<T>()>&>;
};




}