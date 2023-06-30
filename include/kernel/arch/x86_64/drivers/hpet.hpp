#pragma once

#include <stdint.h>
#include <frg/array.hpp>
#include <drivers/acpi.hpp>
#include <frg/array.hpp>
#include <functional>
#include <system/cpu.hpp>
#include <utility>
#include "specs/acpi.hpp"
#include "system/interrupts.hpp"

namespace kernel::drivers::hpet {
enum modes {
    PERIODIC,
    ONESHOT,
};

struct hpet {
    uint64_t cap;
    uint64_t reserved0;
    uint64_t command;
    uint64_t reserved1;
    uint64_t ist;
    uint64_t reserved3[25];
    uint64_t main_counter;
    uint64_t reserved4;

    struct {
        uint64_t command;
        uint64_t value;
        uint64_t fsb;
        uint64_t reserved;
    } __attribute__((packed)) comparators[];
} __attribute__((packed));

class device;

class comparator {
    friend class device;

   private:
    device* device;
    uint8_t num;

    bool fsb;
    bool periodic;

    uint32_t int_route;
    uint8_t vector;

    enum int_modes {
        INT_FSB,
        INT_IOAPIC,
        INT_LEGACY,
        INT_NONE,
    };

    int_modes int_mode = INT_NONE;
    modes mode;

    std::function<void()> func;

    void _start_timer(uint64_t ns);

   public:
    template <typename Func, typename... Args>
    bool start_timer(uint64_t ns, modes mode, Func&& func, Args&&... args) {
        if(this->int_mode == INT_NONE) {
            return false;
        }

        if(mode == PERIODIC && this->periodic == false) {
            return false;
        }

        if(static_cast<bool>(this->func) != false) {
            return false;
        }

        this->mode = mode;
        this->func = [func = std::forward<Func>(func), ...args = std::forward<Args>(args)] mutable {
            func(args...);
        };

        this->_start_timer(ns);
        return true;
    }

    inline bool supports_periodic() {
        return this->periodic;
    }

    void cancel_timer();
};

class device {
    friend class comparator;

    private:
    volatile hpet* regs;

    uint8_t comp_count;
    uint64_t clk;

    bool legacy;

    frg::array<comparator, 32> comps;

    void start();
    void stop();

    public:
    device(acpi_hpet_header_t* table);

    void nsleep(uint64_t ns);
    void msleep(uint64_t ms);

    uint64_t time_ns();
    uint64_t time_ms();
};

extern std::vector<comparator*> comparators;
extern std::vector<device*> devices;

extern bool initialized;

void nsleep(uint64_t ns);
void msleep(uint64_t ms);

uint64_t time_ns();
uint64_t time_ms();

template<typename Func, typename... Args>
static comparator* start_timer(uint64_t ns, modes mode, Func&& func, Args&&... args) {
    for(auto& comp : comparators) {
        if(mode == PERIODIC && comp->supports_periodic() == false) {
            continue;
        }

        if(comp->start_timer(ns, mode, func, args...)) {
            return comp;
        }
    }

    return nullptr;
}

void cancel_timer(comparator* comp);
void init();
}  // namespace kernel::drivers::hpet