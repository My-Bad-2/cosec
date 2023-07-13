#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <debug/log.hpp>
#include <memory/physical.hpp>
#include <system/cpu.hpp>
#include <system/simd.hpp>

namespace system::simd {
using namespace kernel::memory;
using namespace system::cpu;

// static __attribute__((aligned(physical::PAGE_SIZE)))
// uint8_t initial_ctx[physical::PAGE_SIZE] = {};

void init_xcr0() {
    uint64_t xcr0 = 0;

    xcr0 |= XCR0_XSAVE_SAVE_X87;
    xcr0 |= XCR0_XSAVE_SAVE_SSE;

    uint32_t a = 0, b = 0, c = 0, d = 0;

    if (id(CPUID_FEATURE_IDENTIFIER, 0, a, b, c, d)) {
        if (c & CPUID_AVX_SUPPORT) {
            xcr0 |= XCR0_AVX_ENABLE;
        }
    }

    a = b = c = d = 0;

    if (id(CPUID_EXTENDED_FEATURE_IDENTIFIER, 0, a, b, c, d)) {
        if (b & CPUID_AVX512_SUPPORT) {
            xcr0 |= XCR0_AVX512_ENABLE;
            xcr0 |= XCR0_ZMM0_15_ENABLE;
            xcr0 |= XCR0_ZMM16_32_ENABLE;
        }
    }

    write_xcr(0, xcr0);
};

void init() {
    write_cr0(read_cr0() & ~(static_cast<uint64_t>(CR0_EMULATION)));
    write_cr0(read_cr0() | CR0_MONITOR_CO_PROCESSOR);
    write_cr0(read_cr0() | CR0_NUMERIC_ERROR_ENABLE);

    write_cr4(read_cr4() | CR4_FXSR_ENABLE);
    write_cr4(read_cr4() | CR4_SIMD_EXCEPTION_SUPPORT);

    uint32_t a = 0, b = 0, c = 0, d = 0;

    if (id(CPUID_FEATURE_IDENTIFIER, 0, a, b, c, d)) {
        if ((c & CPUID_XSAVE_SUPPORT) == CPUID_XSAVE_SUPPORT) {
            log::debug << "XSAVE Detected\n";

            write_cr4(read_cr4() | CR4_XSAVE_ENABLE);
            init_xcr0();
        }
    }

    log::debug << "SIMD save buffer_size = " << ctx_size() << '\n';
    fninit();
    // ctx_save(initial_ctx);
}

size_t ctx_size() {
    uint32_t a = 0, b = 0, c = 0, d = 0;

    if (id(CPUID_FEATURE_IDENTIFIER, 0, a, b, c, d)) {
        if ((c & CPUID_XSAVE_SUPPORT) == CPUID_XSAVE_SUPPORT) {
            a = b = c = d = 0;

            id(CPUID_PROC_EXTENDED_STATE_ENUMERATION, 0, a, b, c, d);

            return c;
        }
    }

    return 512;
}

void ctx_init(void* ptr) {
    // memcpy(ptr, initial_ctx, ctx_size());
    fninit();
    ctx_save(ptr);
}

void ctx_save(void* ptr) {
    uint32_t a = 0, b = 0, c = 0, d = 0;

    if (id(CPUID_FEATURE_IDENTIFIER, 0, a, b, c, d)) {
        if ((c & CPUID_XSAVE_SUPPORT) == CPUID_XSAVE_SUPPORT) {
            xsave(ptr);
        }
    } else {
        fxsave(ptr);
    }
}

void ctx_load(void* ptr) {
    uint32_t a = 0, b = 0, c = 0, d = 0;

    if (id(CPUID_FEATURE_IDENTIFIER, 0, a, b, c, d)) {
        if ((c & CPUID_XSAVE_SUPPORT) == CPUID_XSAVE_SUPPORT) {
            xrstor(ptr);
        }
    } else {
        fxrstor(ptr);
    }
}
}  // namespace system::simd