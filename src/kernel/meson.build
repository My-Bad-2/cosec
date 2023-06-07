src = files(
    'main.cpp'
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

kernel_elf = executable('cosec.elf', src,
    dependencies: [deps],
    include_directories: incs,
    c_args: [c_args, args],
    cpp_args: [cxx_args, args],
    link_args: ld_args,
    install: false
)