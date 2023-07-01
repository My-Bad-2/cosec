project('limine_terminal', 'c')

sources = files(
    'source/gterm.c',
    'source/image.c',
    'source/term.c',
    'source/tterm.c'
)

include = [
    include_directories('source/'),
    include_directories('fonts/'),
]

limine_term_dep = declare_dependency(include_directories: [include_directories('source'), include_directories('fonts')], sources: sources)

if meson.version().version_compare('>=0.54.0')
    meson.override_dependency('limine_term', limine_term_dep)
endif