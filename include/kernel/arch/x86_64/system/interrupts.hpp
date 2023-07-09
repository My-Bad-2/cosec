#pragma once

#include <stdint.h>
#include <functional>
#include <system/cpu.hpp>
#include <utility>
#include <utils>

namespace system::idt {
class interrupt_handler {
   private:
    std::function<void(cpu::register_t*)> handler;
    bool reserved = false;

   public:
    bool eoi_first = false;

    template <typename Func, typename... Args>
    bool set(Func&& func, Args&&... args) {
        if (this->get()) {
            return false;
        }

        this->handler = [func = std::forward<Func>(func),
                         ... args = std::forward<Args>(args)](
                            cpu::register_t* regs) mutable {
            func(regs, args...);
        };

        return true;
    }

    bool is_reversed() { return this->reserved; }

    bool reserve() {
        if (this->is_reversed()) {
            return false;
        }

        return this->reserved = true;
    }

    bool clear() {
        bool ret = static_cast<bool>(this->handler);
        this->handler.clear();

        return ret;
    }

    bool get() { return bool(this->handler); }

    bool operator()(cpu::register_t* regs) {
        if (this->get() == false) {
            return false;
        }

        this->handler(regs);
        return true;
    }
};

constexpr inline uint8_t IRQ(uint8_t num) {
    return num + 0x20;
}

std::pair<interrupt_handler&, uint8_t> allocate_handler(uint8_t hint = IRQ(0));

extern interrupt_handler handlers[256];
}  // namespace system::idt