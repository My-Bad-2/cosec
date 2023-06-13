#include <cpuid.h>
#include <cstdint>
#include <system/cpu.hpp>

namespace system::cpu {
enum PAT : uint64_t {
    UNCACHABLE = 0x00,
    WRITE_THROUGH = 0x01,
    WRITE_PROTECTED = 0x04,
    WRITE_COMBINING = 0x05,
    WRITE_BACK = 0x06,
    UNCACHED = 0x07
};

constexpr uint64_t custom_pat_config =
    UNCACHABLE | (WRITE_COMBINING << 8) | (WRITE_THROUGH << 32) |
    (WRITE_PROTECTED << 40) | (WRITE_BACK << 48) | (UNCACHED << 56);

bool id(uint32_t leaf, uint32_t subleaf, uint32_t& eax, uint32_t& ebx,
        uint32_t& ecx, uint32_t& edx) {
    uint32_t cpuid_max = 0;

    asm volatile("cpuid"
                 : "=a"(cpuid_max)
                 : "a"(leaf & 0x80000000)
                 : "ebx", "ecx", "edx");

    if (leaf > cpuid_max) {
        return false;
    }

    asm volatile("cpuid"
                 : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                 : "a"(leaf), "c"(subleaf));

    return true;
}

uint64_t rdmsr(uint32_t msr) {
    uint32_t edx, eax;
    asm volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(msr) : "memory");
    return (static_cast<uint64_t>(edx) << 32) | eax;
}

void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t edx = value >> 32;
    uint32_t eax = static_cast<uint32_t>(value);
    asm volatile("wrmsr" ::"a"(eax), "d"(edx), "c"(msr) : "memory");
}

void enable_pat() {
    wrmsr(0x277, custom_pat_config);
}

void invlpg(uintptr_t addr) {
    asm volatile("invlpg (%0)" ::"r"(addr));
}

uintptr_t read_cr3() {
    uintptr_t value;
    asm volatile("mov %%cr3, %0" : "=r"(value)::"memory");
    return value;
}

void write_cr3(uintptr_t value) {
    asm volatile("mov %0, %%cr3" ::"r"(value) : "memory");
}
}  // namespace system::cpu