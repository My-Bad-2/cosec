src += files(
    'system/gdt/gdt.asm',
    'system/gdt/gdt.cpp',
    'system/port/port.cpp',
    'system/system.cpp',

    'drivers/serials.cpp',

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