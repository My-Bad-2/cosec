src = files(
    'main.cpp',
    'sys_info.cpp',

    'common/mmio.cpp',

    'cxx/support.cpp',

    'console/console.cpp',

    'debug/log.cpp',

    'drivers/drivers.cpp',
    'drivers/framebuffer.cpp',
    'drivers/acpi.cpp',
    'drivers/lai.cpp',
    'drivers/pci_bar.cpp',
    'drivers/pci_descriptors.cpp',
    'drivers/pci.cpp',

    'libc/string.cpp',
    'libc/stdlib.cpp',
    'libc/ctype.cpp',
    'libc/assert.cpp',

    'memory/heap.cpp',
    'memory/memory.cpp',
    'memory/physical.cpp',
    'memory/virtual.cpp',

    'time/time.cpp'
)

c_args = get_option('kernel_cflags').split()
cxx_args = get_option('kernel_cxxflags').split()

ld_args = [
    '-Wl,-T' + meson.current_source_dir() / 'linker-' + arch + '.ld',
    '-Wl,-z,max-page-size=0x1000',
    '-nostdlib',
    '-static'
]

args = []

subdir('arch' / arch)

kernel_elf = executable('cosec.elf', src,
    dependencies: [deps],
    include_directories: incs,
    c_args: [c_args, args],
    cpp_args: [cxx_args, args],
    link_args: ld_args,
    install: false
)