#include <assert.h>
#include <lai/host.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstdint>
#include <debug/log.hpp>

#include <arch.hpp>

#include <algorithm>
#include <memory/memory.hpp>
#include <memory/physical.hpp>
#include <memory/virtual.hpp>

#include <drivers/acpi.hpp>

extern "C" {
void laihost_log(int level, const char* msg) {
    if (level == LAI_DEBUG_LOG) {
        log::debug << msg << '\n';
    } else if (level == LAI_WARN_LOG) {
        log::warn << msg << '\n';
    }
}

__attribute__((noreturn)) void laihost_panic(const char* msg) {
    log::error << msg << '\n';

    __builtin_unreachable();
}

void* laihost_malloc(size_t size) {
    return malloc(size);
}

void* laihost_realloc(void* oldptr, size_t newsize, size_t oldsize) {
    (void)oldsize;
    return realloc(oldptr, newsize);
}

void laihost_free(void* ptr, size_t size) {
    (void)size;
    free(ptr);
}

using namespace kernel::memory;

void* laihost_map(size_t address, size_t count) {

    auto pow_to_ceil = [](uint64_t s) {
        assert(s);
        return 1 << (sizeof(uint64_t) * CHAR_BIT - __builtin_clzl(s - 1));
    };

    uintptr_t phys_addr = align_down(address, physical::PAGE_SIZE);
    size_t virt_size = count + (address & (physical::PAGE_SIZE - 1));
    size_t size =
        pow_to_ceil(std::max(virt_size, static_cast<size_t>(0x10000)));

    auto ptr = malloc(size);

    virt::kernel_pagemap->map_range(reinterpret_cast<uintptr_t>(ptr), phys_addr,
                                    virt_size, virt::WRITE, virt::MMIO);

    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) +
                                   (address & (physical::PAGE_SIZE - 1)));
}

void laihost_unmap(void* ptr, size_t size) {
    uintptr_t virt_addr =
        align_down(reinterpret_cast<uintptr_t>(ptr), physical::PAGE_SIZE);
    size_t virt_size =
        size + (reinterpret_cast<uintptr_t>(ptr) & (physical::PAGE_SIZE - 1));

    virt::kernel_pagemap->unmap_range(virt_addr, virt_size);
}

#ifdef __x86_64__
#include <system/port.hpp>

using namespace system::port;

void laihost_outb(uint16_t port, uint8_t val) {
    out8(port, val);
}

void laihost_outw(uint16_t port, uint16_t val) {
    out16(port, val);
}

void laihost_outd(uint16_t port, uint32_t val) {
    out32(port, val);
}

uint8_t laihost_inb(uint16_t port) {
    return in8(port);
}

uint16_t laihost_inw(uint16_t port) {
    return in16(port);
}

uint32_t laihost_ind(uint16_t port) {
    return in32(port);
}
#endif

void laihost_sleep(uint64_t ms) {
    (void)ms;
    log::warn << "Sleep Not implemented yet\n";
}

uint64_t laihost_timer() {
    log::warn << "Timer not implemented yet\n";

    return 0;
}

void* laihost_scan(const char* signature, size_t index) {
    return kernel::drivers::acpi::find_table(signature, index);
}
}