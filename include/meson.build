# Select which files are to be install together with the library

install_headers(
    'gul.h'
)

install_headers(
    'gul/string_view.h',
    subdir : 'gul'
)

# For building the library we use all headers:
inc = [ include_directories([
    '.',
    'gul'
])]
