src += files(
    'system/cpu/cpu.cpp',

    'system/gdt/gdt.asm',
    'system/gdt/gdt.cpp',
    
    'system/interrupts/idt.asm',
    'system/interrupts/idt.cpp',
    'system/interrupts/interrupts.asm',
    'system/interrupts/interrupts.cpp',
    'system/interrupts/pic.cpp',
    'system/lapic/lapic.cpp',
    'system/ioapic/ioapic.cpp',

    'system/port/port.cpp',

    'system/system.cpp',

    'drivers/serials.cpp',
    'drivers/pci.cpp',
    'drivers/pit.cpp',
    'drivers/rtc.cpp',
    'drivers/hpet.cpp',

    'memory/virtual.cpp',

    'arch.cpp'
)

args += [
    '-march=x86-64',
    '-msoft-float',
    '-mno-mmx',
    '-mno-sse',
    '-mno-sse2',
    '-mno-red-zone',
    '-mcmodel=kernel'
]

ld_args += ['-Wl,-z,noexecstack']