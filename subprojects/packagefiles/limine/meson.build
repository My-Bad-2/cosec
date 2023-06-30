project('limine', 'c', license: 'BSD-2-Clause')

this_dir = meson.current_source_dir()

binaries = [
    this_dir / 'limine-bios.sys',
    this_dir / 'limine-bios-cd.bin',
    this_dir / 'limine-uefi-cd.bin'
]

run_command(meson.get_compiler('c').get_id(), this_dir / 'limine.c', '-I.', '-std=c99', '-O2', '-D__USE_MINGW_ANSI_STDIO', '-o', this_dir / 'limine', check : true)
deploy = this_dir / 'limine'

limine_dep = declare_dependency(include_directories: include_directories('.'))

if meson.version().version_compare('>=0.54.0')
    meson.override_dependency('limine', limine_dep)
endif