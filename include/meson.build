# Select which files are to be installed together with the library

libgul_main_header = files([
    'gul.h'
])

install_headers(libgul_main_header)

subdir(meson.project_name())

# For building the library we use all headers:
inc = [ include_directories([
    '.',
])]

# Compatibility header for old projects using <gul/catch.h>,
# will be removed later when all project use <gul14/catch.h> directly instead.
install_headers(
    'gul/catch.h',
    subdir : 'gul'
)

# vi:ts=4:sw=4:sts=4:et
