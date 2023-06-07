project('ovmf_binaries', license: 'BSD-3')

this_dir = meson.current_source_dir()

X64 = this_dir / 'bin/RELEASEX64_OVMF.fd'
x64 = this_dir / 'bin/RELEASEX64_OVMF.fd'


ovmf_binaries_dep = declare_dependency()

if meson.version().version_compare('>=0.54.0')
meson.override_dependency('ovmf_binaries', ovmf_binaries_dep)
endif