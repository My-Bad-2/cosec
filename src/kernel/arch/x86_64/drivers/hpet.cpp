#include <kernel.h>
#include <stddef.h>
#include <stdint.h>

#include <utils>
#include <vector>

#include <debug/log.hpp>

#include <drivers/acpi.hpp>
#include <drivers/hpet.hpp>
#include <drivers/pci.hpp>
#include <drivers/pit.hpp>

#include <memory/memory.hpp>

#include <system/idt.hpp>
#include <system/interrupts.hpp>
#include <system/ioapic.hpp>

#include <time/time.hpp>

namespace kernel::drivers::hpet {
std::vector<comparator*> comparators;
std::vector<device*> devices;

bool initialized = false;

void comparator::_start_timer(uint64_t ns) {
    this->device->stop();

    auto& comp = this->device->regs->comparators[this->num];
    comp.command &= ~((1 << 2) | (1 << 3) | (1 << 6));
    this->device->regs->ist = (1 << this->num);

    uint64_t delta = (ns * 1000000) / this->device->clk;
    if (this->mode == PERIODIC) {
        comp.command |= (1 << 2) | (1 << 3) | (1 << 6);
        comp.value = delta;
    } else {
        comp.command |= (1 << 2);
        comp.value = this->device->regs->main_counter + delta;
    }

    this->device->start();
}

void comparator::cancel_timer() {
    if (this->int_mode == INT_NONE) {
        return;
    }

    auto& comp = this->device->regs->comparators[this->num];
    comp.command &= ~(1 << 2);

    this->func.clear();
}

void device::start() {
    this->regs->command |= (1 << 0);
}

void device::stop() {
    this->regs->command &= ~(1 << 0);
}

device::device(acpi_hpet_header_t* table)
    : regs(reinterpret_cast<hpet*>(
          memory::to_higher_half(table->address.address))) {
    this->stop();

    this->regs->command &= ~0b10;
    this->clk = this->regs->cap >> 32;

    this->comp_count = ((this->regs->cap >> 8) & 0x1F) + 1;
    this->regs->main_counter = 0;
    this->legacy = (this->regs->cap >> 8) & 1;

    log::debug << "Found HPET device " << table->hpet_number
               << ": Legacy replacement mode: "
               << (this->legacy ? "true\n" : "false\n");
    log::debug << " Timers:\n";

    uint32_t gsi_mask = 0xFFFFFFFF;

    for (size_t i = 0; i < this->comp_count; i++) {
        comparator& timer = this->comps[i];

        timer.fsb = (this->regs->comparators[i].command >> 15) & 1;
        timer.periodic = (this->regs->comparators[i].command >> 4) & 1;
        timer.int_route = (this->regs->comparators[i].command >> 32);

        log::debug << " Comparator " << i
                   << ": FSB: " << (timer.fsb ? "true" : "false")
                   << ", Periodic: " << (timer.periodic ? "true\n" : "false\n");

        gsi_mask &= timer.int_route;
    }

    uint32_t gsi = 0xFFFFFFFF;
    uint8_t gsi_vector = 0;

    using namespace system;

    for (size_t i = 0; i < this->comp_count; i++) {
        auto& timer = this->comps[i];

        timer.int_mode = timer.INT_NONE;
        timer.device = this;
        timer.num = i;

        this->regs->comparators[i].command &=
            ~((1 << 14) | (0x1F << 9) | (1 << 8) | (1 << 6) | (1 << 3) |
              (1 << 2) | (1 << 1));

        if (timer.fsb) {
            auto [handler, vector] = idt::allocate_handler();
            handler.set([this, &timer](idt::Regs*) {
                if (static_cast<bool>(timer.func) == false) {
                    return;
                }

                timer.func();

                if (timer.mode != PERIODIC) {
                    this->regs->comparators[timer.num].command &= ~(1 << 2);
                    timer.func.clear();
                }
            });

            timer.vector = vector;

            msi::address address = {
                .base_address = 0xFEE,
                .destination_id = static_cast<uint8_t>(
                    smp_request.response->cpus[0]->lapic_id)};

            msi::data data = {.delivery_mode = 0x00, .vector = vector};

            this->regs->comparators[i].fsb =
                (static_cast<uint64_t>(address.raw) << 32) | data.raw;
            this->regs->comparators[i].command |= (1 << 14);
            timer.int_mode = timer.INT_FSB;
        } else if (ioapic::initialized && std::popcount(gsi_mask) <= 24) {
            if (gsi == 0xFFFFFFFF) {
                for (size_t g = 31; g >= 0; g--) {
                    if (gsi_mask & (1 << g)) {
                        if (ioapic::ioapic_for_gsi(g) == nullptr) {
                            continue;
                        }

                        gsi = g;
                        break;
                    }
                }

                gsi_mask = gsi + 0x20;

                idt::handlers[gsi_vector].set([this](idt::Regs*) {
                    for (size_t i = 0; i < this->comp_count; i++) {
                        if (this->regs->ist & (1 << i)) {
                            comparator& timer = this->comps[i];

                            if (static_cast<bool>(timer.func) == false) {
                                return;
                            }

                            timer.func();

                            if (timer.mode != PERIODIC) {
                                this->regs->comparators[timer.num].command &=
                                    ~(1 << 2);
                                timer.func.clear();
                            }

                            this->regs->ist = (1 << i);
                        }
                    }
                });

                ioapic::set(gsi, gsi_vector, ioapic::delivery_mode::FIXED,
                            ioapic::dest_mode::PHYSICAL,
                            ioapic::flags::EDGE_LEVEL,
                            smp_request.response->cpus[i]->lapic_id);
            }

            this->regs->comparators[i].command |=
                ((gsi & 0x1F) << 9) | (1 << 1);

            timer.vector = gsi_vector;
            timer.int_mode = timer.INT_IOAPIC;
        } else {
            log::error << "HPET: Neither standard nor FSB interrupt mappings "
                          "are supported.\n";
            continue;
        }

        comparators.push_back(&this->comps[i]);
    }

    this->start();
}

void device::nsleep(uint64_t ns) {
    uint64_t target = this->regs->main_counter + ((ns * 1000000) / this->clk);

    while (this->regs->main_counter < target) {
        asm volatile("pause");
    }
}

void device::msleep(uint64_t ms) {
    uint64_t target =
        this->regs->main_counter + ((ms + 1000000000000) / this->clk);

    while (this->regs->main_counter < target) {
        asm volatile("pause");
    }
}

uint64_t device::time_ns() {
    return (this->regs->main_counter * this->clk) / 1000000;
}

uint64_t device::time_ms() {
    return (this->regs->main_counter * this->clk) / 1000000000000;
}

void nsleep(uint64_t ns) {
    devices.front()->nsleep(ns);
}

void msleep(uint64_t ms) {
    devices.front()->msleep(ms);
}

uint64_t time_ns() {
    return devices.front()->time_ns();
}

uint64_t time_ms() {
    return devices.front()->time_ms();
}

void cancel_timer(comparator* comp) {
    comp->cancel_timer();
}

void init() {
    auto table =
        reinterpret_cast<acpi_hpet_header_t*>(acpi::find_table("HPET", 0));

    if (table == nullptr) {
        log::error << "HPET table not found\n";
        return;
    }

    for (size_t i = 0; table != nullptr;
         i++, table = reinterpret_cast<acpi_hpet_header_t*>(
                  acpi::find_table("HPET", i))) {
        devices.push_back(new device(table));
    }

    asm("cli");
    static constexpr size_t ns = 1000000;

    using namespace system;

    if (start_timer(ns, PERIODIC, [] { time::timer_handler(ns); })) {
        idt::mask(pit::vector - 0x20);
    }

    asm("sti");

    initialized = true;

    log::info << "HPET intialized\n";
}
}  // namespace kernel::drivers::hpet