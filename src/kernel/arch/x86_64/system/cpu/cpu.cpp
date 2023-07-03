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

uintptr_t read_cr0() {
    uintptr_t value;
    asm volatile("mov %%cr0, %0" : "=r"(value)::"memory");
    return value;
}

uintptr_t read_cr2() {
    uintptr_t value;
    asm volatile("mov %%cr2, %0" : "=r"(value)::"memory");
    return value;
}

uintptr_t read_cr3() {
    uintptr_t value;
    asm volatile("mov %%cr3, %0" : "=r"(value)::"memory");
    return value;
}

uintptr_t read_cr4() {
    uintptr_t value;
    asm volatile("mov %%cr4, %0" : "=r"(value)::"memory");
    return value;
}

void write_cr0(uintptr_t value) {
    asm volatile("mov %0, %%cr0" ::"r"(value) : "memory");
}

void write_cr2(uintptr_t value) {
    asm volatile("mov %0, %%cr2" ::"r"(value) : "memory");
}

void write_cr3(uintptr_t value) {
    asm volatile("mov %0, %%cr3" ::"r"(value) : "memory");
}

void write_cr4(uintptr_t value) {
    asm volatile("mov %0, %%cr4" ::"r"(value) : "memory");
}

uint64_t read_xcr(uint32_t reg) {
    uint32_t eax, edx;

    asm volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(reg) : "memory");

    return eax | (static_cast<uint64_t>(edx) << 32);
}

void write_xcr(uint32_t reg, uint64_t value) {
    uint32_t edx = value >> 32;
    uint32_t eax = static_cast<uint32_t>(value);

    asm volatile("xsetbv" ::"a"(eax), "d"(edx), "c"(reg) : "memory");
}

static uint64_t rfbm = ~0;
static uint32_t rfbm_low = rfbm & 0xFFFFFFFF;
static uint32_t rfbm_high = (rfbm >> 32) & 0xFFFFFFFF;

void xsave(void* ctx) {
    asm volatile("xsave64 (%0)" ::"r"(ctx), "a"(rfbm_low), "d"(rfbm_high)
                 : "memory");
}

void xrstor(void* ctx) {
    asm volatile("xrstor64 (%0)" ::"r"(ctx), "a"(rfbm_low), "d"(rfbm_high)
                 : "memory");
}

void fxsave(void* ctx) {
    asm volatile("fxsave (%0)" ::"r"(ctx) : "memory");
}

void fxrstor(void* ctx) {
    asm volatile("fxrstor (%0)" ::"r"(ctx) : "memory");
}

uintptr_t get_gs() {
    return rdmsr(0xC0000101);
}

uintptr_t get_kernel_gs() {
    return rdmsr(0xC0000102);
}

void set_gs(uintptr_t address) {
    wrmsr(0xC0000101, address);
}

void set_kernel_gs(uintptr_t address) {
    wrmsr(0xC0000102, address);
}

uintptr_t get_fs() {
    return rdmsr(0xC0000100);
}

void set_fs(uintptr_t address) {
    wrmsr(0xC0000100, address);
}

void enable_interrupts() {
    asm volatile("sti");
}

void disable_interrupts() {
    asm volatile("cli");
}

bool interrupt_state() {
    uint64_t flags;
    asm volatile("pushfq; pop %0" : "=rm"(flags)::"memory");
    return flags & (1 << 9);
}

void halt() {
    asm volatile("hlt");
}

void pause() {
    asm volatile("pause");
}

void fninit() {
    asm volatile("fninit");
}
}  // namespace system::cpu