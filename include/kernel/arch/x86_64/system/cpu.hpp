#pragma once

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <system/gdt.hpp>
#include <system/idt.hpp>
#include <system/lapic.hpp>
#include <system/tss.hpp>

namespace system::cpu {
enum xcr0_bit {
    XCR0_XSAVE_SAVE_X87 = (1 << 0),
    XCR0_XSAVE_SAVE_SSE = (1 << 1),
    XCR0_AVX_ENABLE = (1 << 2),
    XCR0_BNDREG_ENABLE = (1 << 3),
    XCR0_BNDCSR_ENABLE = (1 << 4),
    XCR0_AVX512_ENABLE = (1 << 5),
    XCR0_ZMM0_15_ENABLE = (1 << 6),
    XCR0_ZMM16_32_ENABLE = (1 << 7),
    XCR0_PKRU_ENABLE = (1 << 9),
};

enum cr0_bit {
    CR0_PROTECTED_MODE_ENABLE = (1 << 0),
    CR0_MONITOR_CO_PROCESSOR = (1 << 1),
    CR0_EMULATION = (1 << 2),
    CR0_TASK_SWITCHED = (1 << 3),
    CR0_EXTENSION_TYPE = (1 << 4),
    CR0_NUMERIC_ERROR_ENABLE = (1 << 5),
    CR0_WRITE_PROTECT_ENABLE = (1 << 16),
    CR0_ALIGNMENT_MASK = (1 << 18),
    CR0_NOT_WRITE_THROUGH_ENABLE = (1 << 29),
    CR0_CACHE_DISABLE = (1 << 30),
    CR0_PAGING_ENABLE = (1 << 31),
};

enum cr4_bit {
    CR4_VIRTUAL_8086_MODE_EXT = (1 << 0),
    CR4_PROTECTED_MODE_VIRTUAL_INT = (1 << 1),
    CR4_TIME_STAMP_DISABLE =
        (1 << 2),  // disable it only for ring != 0 for RDTSC instruction
    CR4_DEBUGGING_EXT = (1 << 3),  // enable debug register break on io space
    CR4_PAGE_SIZE_EXT = (1 << 4),
    CR4_PHYSICAL_ADDRESS_EXT = (1 << 5),
    CR4_MACHINE_CHECK_EXCEPTION = (1 << 6),
    CR4_PAGE_GLOBAL_ENABLE = (1 << 7),
    CR4_PERFORMANCE_COUNTER_ENABLE = (1 << 8),
    CR4_FXSR_ENABLE = (1 << 9),
    CR4_SIMD_EXCEPTION_SUPPORT =
        (1
         << 10),  // Operating System Support for Unmasked SIMD Floating-Point Exceptions
    CR4_USER_MODE_INSTRUCTION_PREVENTION = (1 << 11),
    CR4_5_LEVEL_PAGING_ENABLE = (1 << 12),
    CR4_VIRTUAL_MACHINE_EXT_ENABLE = (1 << 13),
    CR4_SAFER_MODE_EXT_ENABLE = (1 << 14),
    CR4_FS_GS_BASE_ENABLE = (1 << 16),
    CR4_PCID_ENABLE = (1 << 17),
    CR4_XSAVE_ENABLE = (1 << 18),
    CR4_SUPERVISOR_EXE_PROTECTION_ENABLE = (1 << 20),
    CR4_SUPERVISOR_ACCESS_PROTECTION_ENABLE = (1 << 21),
    CR4_KEY_PROTECTION_ENABLE = (1 << 22),
    CR4_CONTROL_FLOW_ENABLE = (1 << 23),
    CR4_SUPERVISOR_KEY_PROTECTION_ENABLE = (1 << 24),
};

enum cpuid_leaf {
    CPUID_FEATURE_IDENTIFIER = 1,
    CPUID_EXTENDED_FEATURE_IDENTIFIER = 7,
    CPUID_PROC_EXTENDED_STATE_ENUMERATION = 13
};

enum cpuid_feature_bits {
    // ECX
    CPUID_SSSE3_SUPPORT = (1 << 9),
    CPUID_SSE41_SUPPORT = (1 << 19),
    CPUID_SSE42_SUPPORT = (1 << 20),
    CPUID_AES_SUPPORT = (1 << 25),
    CPUID_XSAVE_SUPPORT = (1 << 26),
    CPUID_XSAVE_ENABLED = (1 << 27),
    CPUID_AVX_SUPPORT = (1 << 28),
};

enum cpuid_extended_feature_bits {
    // EBX
    CPUID_BIT_MANIPULATION_SUPPORT = (1 << 3),
    CPUID_AVX512_SUPPORT = (1 << 16),
};

bool id(uint32_t leaf, uint32_t subleaf, uint32_t& eax, uint32_t& ebx,
        uint32_t& ecx, uint32_t& edx);

uint64_t rdmsr(uint32_t msr);
void wrmsr(uint32_t msr, uint64_t value);

void invlpg(uintptr_t addr);

void enable_pat();
void enable_sse();

uintptr_t read_cr0();
uintptr_t read_cr2();
uintptr_t read_cr3();
uintptr_t read_cr4();

void write_cr0(uintptr_t value);
void write_cr2(uintptr_t value);
void write_cr3(uintptr_t value);
void write_cr4(uintptr_t value);

uint64_t read_xcr(uint32_t reg);
void write_xcr(uint32_t reg, uint64_t value);

uintptr_t get_gs();
uintptr_t get_kernel_gs();

void set_gs(uintptr_t address);
void set_kernel_gs(uintptr_t address);

uintptr_t get_fs();
void set_fs(uintptr_t address);

void xsave(void* ctx);
void xrstor(void* ctx);
void fxsave(void* ctx);
void fxrstor(void* ctx);

void halt();
void pause();
bool interrupt_state();
void enable_interrupts();
void disable_interrupts();
void interrupt_toggle(bool status);

void fninit();

struct register_t {
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

struct cpu_t {
    cpu_t* self = this;  // pointer to this structure
    uint64_t id;         // APIC/CPU id

    uint64_t arch_id = 0;

    tss::tss_t tss __attribute__((aligned(16)));

    gdt::gdt_t* gdt;  // gdt
    gdt::gdt_descriptor_t gdt_ptr;
    idt::idt_descriptor_t idt_ptr;

    lapic::lapic lapic;
    uint64_t lapic_ticks_per_ms;

    errno_t error;
    bool is_up = false;
} __attribute__((packed));
}  // namespace system::cpu