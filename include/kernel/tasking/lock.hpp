#pragma once

#include <arch.hpp>
#include <atomic>
#include <mutex>

namespace kernel {
class irq_lock {
   private:
    bool m_irqs = false;
    std::mutex m_lock;

   public:
    constexpr irq_lock() : m_irqs(false), m_lock() {}

    irq_lock(const irq_lock&) = delete;
    irq_lock& operator=(const irq_lock&) = delete;

    void lock() {
        auto irqs = interrupt_state();
        interrupt_toggle(false);

        this->m_lock.lock();
        this->m_irqs = irqs;
    }

    void unlock() {
        bool irqs = this->m_irqs;
        this->m_lock.unlock();

        interrupt_toggle(irqs);
    }

    bool is_locked() { return this->m_lock.is_locked(); }

    bool try_lock() {
        if (this->is_locked()) {
            return false;
        }

        this->lock();
        return true;
    }
};
}  // namespace kernel