#pragma once

#include "storage/storage.hpp"
#include "storage/localStorage.hpp"
#include "core/memory.hpp"
#include "core/config.hpp"

namespace mkr {


template <typename Aggregator, template<size_t maxMem> typename Storage = LocalStorage, IsConfig ConfigParam = DefaultConfig>
class Maker final : public Storage<details::getSize<Aggregator>()>, private ConfigParam {
public:
    using Config = ConfigParam;

    static_assert(IsStorage<Storage<details::getSize<Aggregator>()>>);

    template <typename ...Args>
    constexpr explicit Maker(Args&& ...args)
        : Config(std::forward<Args>(args)...) {

        init();
    }

    void init() {
        for (auto& memory : this->allMemory()) {
            m_pattern.initMemory(memory, config());
            memory.setInitialized(true);
        }
    }

    template <typename ...Args>
    std::string_view generate(Args&& ...args) {
        return m_pattern.generate(this->getMemory(), config(), std::forward<Args>(args)...);
    }
private:
    Config const& config() const noexcept {
        return static_cast<Config const&>(*this);
    }

    Aggregator m_pattern{};
};

}