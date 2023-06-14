project('cosec', ['cpp', 'c', 'nasm'],
  version : '0.1',
  default_options : ['warning_level=3'],
  license: 'MIT')

c = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')
ld = cxx.get_linker_id()
arch = host_machine.cpu_family()

logfile = meson.source_root() / 'debug/log'

iso_dir = meson.source_root() / '/tmp/iso_dir'
iso_files = [
    meson.current_source_dir() / 'res/limine.cfg'
]

xorriso_flags = [
    '-as', 'mkisofs',
    '-b', 'boot/limine/limine-cd.bin',
    '-no-emul-boot',
    '-boot-load-size', '4',
    '-boot-info-table',
    '--efi-boot', 'boot/limine/limine-cd-efi.bin',
    '-efi-boot-part',
    '--efi-boot-image',
    '--protective-msdos-label'
]

qemu_args = [
    '-cpu', 'max', '-smp', '4', '-m', '512M',
    '-rtc', 'base=localtime', '-serial', 'stdio',
    '-boot', 'order=d,menu=on,splash-time=100'
]

supported_archs = {
    'x86_64' : [
        'x64',
        true, # supports bios?
        [
            '-M', 'q35'
        ]
    ]
}

if not supported_archs.has_key(arch)
error('Unknown architecture ' + arch)
endif

ovmf_id = supported_archs[arch][0]
supports_bios = supported_archs[arch][1]
qemu_args += supported_archs[arch][2]
qemu = 'qemu-system-' + arch

if get_option('vnc')
    qemu_args += ['-vnc', '127.0.0.1:1']
endif

lldb = get_option('lldb')
if get_option('lldb_debug') or lldb
qemu_args += [
    '-no-reboot', '-no-shutdown',
    '-d', 'int', '-D', logfile,
    # '-monitor', 'telnet:127.0.0.1:12345,nowait'
]

if arch == 'x86_64'
qemu_args += ['-M', 'smm=off']
endif

if lldb
qemu_args += ['-s', '-S']
endif
endif

asm_format = ''

if arch =='x86_64'
asm_format = 'elf64'
endif

incs = [
    include_directories('include'),
    include_directories('include/kernel'),
    include_directories('include/libc'),
    include_directories('include/libc++'),
    include_directories('include/kernel/arch/' + arch)
]

macros = [
    '-DDEBUG=' + (get_option('lldb_debug') ? '1' : '0')
]

warnings = [
    '-Wno-error=#warnings',
    '-Wno-builtin-macro-redefined',
    '-Wno-macro-redefined'
]

add_global_arguments([
    warnings, macros,
    '-ffreestanding',
    '-fno-stack-protector',
    '-fno-omit-frame-pointer',
    '-fno-pic',
    '-fno-pie',
    '-Ofast'
], language : ['c', 'cpp'])

add_global_arguments(['-f', asm_format], language: 'nasm')

add_global_arguments([
    '-std=gnu++20',
    '-fno-rtti',
    '-fno-exceptions',
    '-fsized-deallocation',
    '-Wno-used-parameter',
    '-Wno-frame-address',
], language : 'cpp')

limine = subproject('limine')

deps = [
    dependency('limine'),
    dependency('printf'),
    dependency('cxxshim'),
    dependency('frigg')
]

inc_deps = []
foreach dep : deps
inc_deps += dep.partial_dependency(includes: true)
endforeach

kernel_elf = []
subdir('src')

iso_files += limine.get_variable('binaries')
iso = custom_target('iso',
    input: [kernel_elf],
    output: 'image.iso',
    command: [
        'mkdir', '-p', iso_dir / 'boot/limine', '&&',
        'cp', '@INPUT@', iso_dir / 'kernel.elf', '&&',
        'cp', iso_files, iso_dir / 'boot/limine', '&&',
        'xorriso', xorriso_flags, iso_dir, '-o', '@OUTPUT@', '&&',
        limine.get_variable('deploy'), '@OUTPUT@', '&&',
        'rm', '-rf', iso_dir
    ],
    build_by_default: true,
    console: true,
    install: false,
    depends: [kernel_elf]
)

ovmf_binaries = subproject('ovmf_binaries')
ovmf = ovmf_binaries.get_variable(ovmf_id)

run_efi = custom_target('run_efi',
    input: iso,
    output: 'run_efi',
    command: [qemu, qemu_args, '-cdrom', '@INPUT@', '-bios', ovmf],
    build_by_default: true,
    console: true,
    install: false,
    depends: iso
)