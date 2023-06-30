#pragma once

#include <stdint.h>
#include <functional>
#include <utility>

namespace system::idt {
struct Regs {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;

    uint64_t int_no;
    uint64_t error_code;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed));

template <typename T, typename U>
struct pair {
    T first;
    U second;
};

class interrupt_handler {
   private:
    std::function<void(Regs*)> handler;
    bool reserved = false;

   public:
    bool eoi_first = false;

    template <typename Func, typename... Args>
    bool set(Func&& func, Args&&... args) {
        if (this->get()) {
            return false;
        }

        this->handler = [func = std::forward<Func>(func),
                         ... args =
                             std::forward<Args>(args)](Regs* regs) mutable {
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

    bool operator()(Regs* regs) {
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

pair<interrupt_handler&, uint8_t> allocate_handler(uint8_t hint = IRQ(0));

extern interrupt_handler handlers[256];
}  // namespace system::idt