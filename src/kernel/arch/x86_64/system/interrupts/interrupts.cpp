#include <stddef.h>
#include <stdint.h>
#include <system/interrupts.hpp>

#include <debug/log.hpp>

namespace system::interrupts {
static const char* exception_messages[32] = {
    "DivisionByZero",
    "Debug",
    "NonMaskableInterrupt",
    "Breakpoint",
    "DetectedOverflow",
    "OutOfBounds",
    "InvalidOpcode",
    "NoCoprocessor",
    "DoubleFault",
    "CoprocessorSegmentOverrun",
    "BadTss",
    "SegmentNotPresent",
    "StackFault",
    "GeneralProtectionFault",
    "PageFault",
    "UnknownInterrupt",
    "CoprocessorFault",
    "AlignmentCheck",
    "MachineCheck",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

struct Stackframe {
    Stackframe* rbp;
    uint64_t rip;
};

static bool has_panic = false;

static size_t dump_backtrace(uintptr_t rbp) {
    Stackframe* stackframe = reinterpret_cast<Stackframe*>(rbp);
    size_t i = 0;

    while (stackframe) {
        log::debug << "- " << stackframe->rip;
        stackframe = stackframe->rbp;
    }

    log::info << "\n";

    return i;
}

void dump_register(const idt::Regs* regs) {
    log::debug << "RIP: " << regs->rip << " | RSP: " << regs->rsp << "\n";
    log::debug << "CS : " << regs->cs << " | SS: " << regs->ss
               << " | RFlags: " << regs->rflags << "\n";

    log::debug << "\n";

    log::debug << "RAX: " << regs->rax << " | RBX: " << regs->rbx << "\n";
    log::debug << "RCX: " << regs->rcx << " | RDX: " << regs->rdx << "\n";
    log::debug << "RSI: " << regs->rsi << " | RDI: " << regs->rdi << "\n";
    log::debug << "RBP: " << regs->rbp << " | R8 : " << regs->r8 << "\n";
    log::debug << "R9 : " << regs->r9 << " | R10: " << regs->r10 << "\n";
    log::debug << "R11: " << regs->r11 << " | R12: " << regs->r12 << "\n";
    log::debug << "R13: " << regs->r13 << " | R14: " << regs->r14 << "\n";
    log::debug << "R15: " << regs->r15 << "\n";
}

static void interrupt_error_handler(idt::Regs* regs) {
    if (has_panic) {
        log::error << "[NESTED PANIC]"
                   << "\n";

        while (true) {
            asm("cli");
            asm("hlt");
        }

        return;
    }

    has_panic = true;

    log::error << "\n---------------------------\n";
    log::error << exception_messages[regs->int_no] << '(' << regs->int_no
               << ") with error code = " << regs->error_code << "!\n";

    dump_register(regs);
    dump_backtrace(regs->rbp);

    log::error << "\n---------------------------\n";

    while (true) {
        asm("cli");
        asm("hlt");
    }
}

extern "C" uint64_t interrupt_handler(uint64_t rsp) {
    idt::Regs* regs = reinterpret_cast<idt::Regs*>(rsp);

    if (regs->int_no < 32) {
        interrupt_error_handler(regs);
    }

    if (regs->int_no == 0xF0) {
        log::debug
            << "Non-maskable Interrupt from APIC(Possible Hardware error).\n";
    }

    return rsp;
}
}  // namespace system::interrupts