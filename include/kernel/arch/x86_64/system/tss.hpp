#pragma once

#include <stddef.h>
#include <stdint.h>
#include <system/gdt.hpp>

namespace system::tss {
struct tss_t {
    uint32_t reserved __attribute__((aligned(16)));
    uint64_t rsp[3];
    uint64_t reserved0;
    uint64_t ist[7];
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
    uint16_t iopb_offset;

    /**
     * @brief Set the kernel stack object
     * 
     * @param stack 
     */
    inline void set_kernel_stack(uint64_t stack) { this->rsp[0] = stack; }
} __attribute__((packed));

/**
 * @brief Initialize Task state Segment
 * 
 * @param tss 
 * @param gdt 
 */
void init(tss_t* tss, gdt::gdt_t* gdt);
}  // namespace system::tss