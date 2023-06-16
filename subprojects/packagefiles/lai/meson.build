project('lai', 'c')

# One easy way to build LAI with custom flags (such as -ffreestanding,
# -mcmodel=kernel, -mno-red-zone etc.)
# is including it as a Meson subproject and using the 'sources' and 'includes'
# variables to build your own LAI library.

sources = files(
    'core/error.c',
    'core/eval.c',
    'core/exec.c',
    'core/exec-operand.c',
    'core/libc.c',
    'core/ns.c',
    'core/object.c',
    'core/opregion.c',
    'core/os_methods.c',
    'core/variable.c',
    'core/vsnprintf.c',
    'helpers/pc-bios.c',
    'helpers/pci.c',
    'helpers/resource.c',
    'helpers/sci.c',
    'helpers/pm.c',
    'drivers/ec.c',
    'drivers/timer.c',
)

lai_dep = declare_dependency(include_directories: include_directories('include'), sources: sources)

if meson.version().version_compare('>=0.54.0')
    meson.override_dependency('lai', lai_dep)
endif