#include <stdint.h>
#include <atomic>

#include <debug/log.hpp>

#include <drivers/pit.hpp>

#include <system/interrupts.hpp>
#include <system/idt.hpp>
#include <system/port.hpp>

#include <time/time.hpp>

#define INPUT_FREQUENCY 1193180  // Hz

#define CHANNEL0 0x40
#define CHANNEL1 0x41
#define CHANNEL2 0x42
#define COMMAND_REGISTER 0x43

namespace kernel::drivers::pit {
static constexpr uint64_t frequency = 1000;
static std::atomic_uint64_t ticks = 0;
uint8_t vector = 0;

uint64_t time_ms() {
    return ticks * (1000 / frequency);
}

void msleep(uint64_t ms) {
    volatile uint64_t target = time_ms() + ms;

    while (time_ms() < target) {
        asm volatile("pause");
    }
}

void init() {
    uint64_t divisor = INPUT_FREQUENCY / frequency;

    using namespace system::port;

    out8(COMMAND_REGISTER, 0x36);

    auto low = static_cast<uint8_t>(divisor);
    auto high = static_cast<uint8_t>(divisor >> 8);

    out8(CHANNEL0, low);
    out8(CHANNEL0, high);

    using namespace system::idt;
    auto [handler, _vector] = allocate_handler();

    handler.set([](Regs*) {
        ticks++;
        time::timer_handler(1000000000 / frequency);
    });

    unmask((vector = _vector) - 0x20);

    log::info << "Initialized PIT!\n";
}

void handler() {
    ticks++;
    time::timer_handler(1000000000 / frequency);
}
}  // namespace kernel::drivers::pit